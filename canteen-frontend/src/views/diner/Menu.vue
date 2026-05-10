<template>
  <div>
    <h2>菜单列表</h2>

    <el-form inline style="margin-bottom: 16px">
      <el-form-item label="餐别">
        <el-select
          v-model="selectedMealType"
          placeholder="请选择餐别"
          style="width: 120px"
          @change="getMenu"
        >
          <el-option label="午餐" value="午餐"/>
          <el-option label="晚餐" value="晚餐"/>
        </el-select>
      </el-form-item>
      <el-form-item label="为谁点餐">
        <el-select
          v-model="selectedOrderForUserId"
          placeholder="请选择家庭成员"
          style="width: 240px"
          @change="handleOrderForUserChange"
        >
          <el-option
            v-for="member in familyMembers"
            :key="member.user_id"
            :label="member.username"
            :value="member.user_id"
          />
        </el-select>
      </el-form-item>
      <el-form-item v-if="selectedUserAge">
        <el-tag>{{ selectedUserAge }}岁 - {{ discountText }}</el-tag>
      </el-form-item>
      <el-form-item>
        <el-button type="warning" @click="openFavoriteDialog">
          {{ isOrderingForSelf ? '我的收藏' : '他的收藏' }}
        </el-button>
      </el-form-item>
    </el-form>

    <!-- 为您推荐 -->
    <div v-if="recommendedDishes.length > 0" style="margin-bottom: 20px;">
      <h3 style="margin-top: 0; margin-bottom: 15px; display: flex; align-items: center;">
        <span style="font-size: 18px; margin-right: 8px;">🎯</span>
        <span>为您推荐</span>
      </h3>
      <div style="display: flex; overflow-x: auto; gap: 15px; padding-bottom: 10px;">
        <div
          v-for="dish in recommendedDishes"
          :key="dish.dishId"
          style="flex: 0 0 280px; background-color: white; border-radius: 8px; box-shadow: 0 2px 12px rgba(0,0,0,0.1); padding: 15px; min-width: 280px;"
          :style="{ backgroundColor: dish.bgColor }"
        >
          <!-- 推荐类型图标 -->
          <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px;">
            <span style="font-size: 24px;">{{ dish.icon }}</span>
            <span style="font-size: 12px; color: #999;">推荐分: {{ dish.recommendationScore }}</span>
          </div>
          
          <!-- 菜品名称 -->
          <h4 style="margin: 0 0 8px 0; font-size: 16px; font-weight: bold;">{{ dish.dishName }}</h4>
          
          <!-- 价格 -->
          <div style="margin-bottom: 8px;">
            <span style="font-size: 18px; color: #f56c6c; font-weight: bold;">¥{{ dish.price.toFixed(2) }}</span>
          </div>
          
          <!-- 标签 -->
          <div style="display: flex; flex-wrap: wrap; gap: 4px; margin-bottom: 10px;">
            <span
              v-for="(tag, index) in dish.tags"
              :key="index"
              style="font-size: 12px; padding: 2px 8px; border-radius: 10px; background-color: white; color: #666;"
            >
              {{ tag }}
            </span>
          </div>
          
          <!-- 推荐原因 -->
          <div style="margin-bottom: 12px;">
            <span
              style="font-size: 12px; padding: 4px 8px; border-radius: 4px; display: inline-block;"
              :style="{ color: dish.color, backgroundColor: dish.bgColor, border: `1px solid ${dish.color}` }"
            >
              {{ dish.recommendationReason }}
            </span>
          </div>
          
          <!-- 添加按钮 -->
          <el-button
            type="primary"
            size="small"
            style="width: 100%;"
            @click="addFromRecommendation(dish)"
          >
            添加到订单
          </el-button>
        </div>
      </div>
    </div>

    <!-- 最近订单信息 -->
    <div v-if="recentOrders.length > 0" style="margin-bottom: 20px;">
      <h3 style="margin-top: 0; margin-bottom: 15px;">最近订单</h3>
      <div class="recent-orders-container">
        <el-card 
          v-for="order in recentOrders" 
          :key="order.order_id"
          :class="['recent-order-card', { active: selectedOrder && selectedOrder.order_id === order.order_id }]"
          @click="selectOrder(order)"
        >
          <div class="order-card-header">
            <span class="order-time">{{ order.order_time }}</span>
            <el-tag size="small" :type="order.order_id === selectedOrder?.order_id ? 'primary' : 'info'">
              {{ order.order_id === selectedOrder?.order_id ? '已选择' : '点击选择' }}
            </el-tag>
          </div>
          <div class="order-dishes">
            <span v-for="(item, index) in order.items.slice(0, 3)" :key="index" class="dish-tag">
              {{ item.dish_name }} × {{ item.quantity }}
            </span>
            <span v-if="order.items.length > 3" class="more-dishes">
              +{{ order.items.length - 3 }} 更多
            </span>
          </div>
          <div class="order-price">
            <span class="price-label">总价:</span>
            <span class="price-value">¥{{ order.total_price.toFixed(2) }}</span>
          </div>
        </el-card>
      </div>
    </div>
    <div v-else style="margin-bottom: 20px; padding: 15px; border: 1px solid #e4e7ed; border-radius: 4px; background-color: #f9f9f9;">
      暂无最近的订单信息
    </div>

    <!-- 已选菜品 -->
    <div v-if="recentOrders.length > 0 && selectedOrder" style="margin-bottom: 20px; padding: 15px; border: 1px solid #e4e7ed; border-radius: 4px; background-color: #f9f9f9;">
      <h3 style="margin-top: 0; margin-bottom: 10px;">已选菜品 - 订单 {{ selectedOrder.order_time }}</h3>
      <div v-if="selectedOrderItems.length > 0">
        <div style="margin-bottom: 10px;">
          <span>菜品：</span>
          <span v-for="(item, index) in selectedOrderItems" :key="index">
            {{ item.dish_name }} × {{ item.quantity }}
            <span v-if="index < selectedOrderItems.length - 1">，</span>
          </span>
        </div>
        <div v-if="missingDishes.length > 0" style="margin-bottom: 10px; color: #e6a23c;">
          <span>⚠️ 当前餐单中暂无菜品：{{ missingDishes.join('、') }}</span>
        </div>
        <el-button type="primary" @click="addToOrder">添加到订单</el-button>
      </div>
      <div v-else>
        暂无已选菜品
      </div>
    </div>

    <div v-if="dishes.length === 0" style="text-align: center; padding: 20px; color: #999;">
      该餐暂无菜品
    </div>

    <el-table v-else :data="dishes" style="width: 100%">
      <el-table-column prop="id" label="ID" width="100" />
      <el-table-column label="菜品名称">
        <template #default="scope">
          <span>{{ scope.row.name }}</span>
          <el-tag v-for="(tag, index) in scope.row.tags" :key="index" size="small" style="margin-left: 8px;">
            {{ tag }}
          </el-tag>
        </template>
      </el-table-column>
      <el-table-column label="价格" width="180">
        <template #default="scope">
          <div>
            <span v-if="discount < 1" style="text-decoration: line-through; color: #999;">
              ¥{{ scope.row.price.toFixed(2) }}
            </span>
            <span v-else>¥{{ scope.row.price.toFixed(2) }}</span>
            <span v-if="discount < 1" style="margin-left: 10px; color: #f56c6c;">
              ¥{{ (scope.row.price * discount).toFixed(2) }}
            </span>
          </div>
        </template>
      </el-table-column>

       <!-- ⭐ 数量选择 -->
      <el-table-column label="数量">
        <template #default="scope">
          <el-input-number
            v-model="scope.row.quantity"
            :min="0"
            :max="10"
          />
        </template>
      </el-table-column>

      <!-- ⭐ 营养信息 -->
      <el-table-column label="营养信息" width="120">
        <template #default="scope">
          <el-button type="info" size="small" @click="showNutritionInfo(scope.row)">
            营养信息
          </el-button>
        </template>
      </el-table-column>

      <!-- ⭐ 收藏 -->
      <el-table-column label="收藏" width="80">
        <template #default="scope">
          <el-button
            :type="scope.row.isFavorite ? 'danger' : 'default'"
            circle
            size="small"
            @click="toggleFavorite(scope.row)"
          >
            <el-icon v-if="scope.row.isFavorite"><StarFilled /></el-icon>
            <el-icon v-else><Star /></el-icon>
          </el-button>
        </template>
      </el-table-column>

    </el-table>

    <!-- ⭐ 营养信息弹窗 -->
    <el-dialog v-model="nutritionDialogVisible" title="营养信息" width="400px">
      <div v-if="selectedDish">
        <el-form :model="selectedDish" label-width="80px">
          <el-form-item label="菜品名称">
            {{ selectedDish.name }}
          </el-form-item>
          <el-form-item label="类型">
            {{ selectedDish.type }}
          </el-form-item>
          <el-form-item label="热量">
            {{ selectedDish.calories }} 卡路里
          </el-form-item>
          <el-form-item label="营养信息">
            {{ selectedDish.nutrition_info || '暂无营养信息' }}
          </el-form-item>
        </el-form>
      </div>
    </el-dialog>

    <!-- ⭐ 收藏弹窗 -->
    <el-dialog v-model="favoriteDialogVisible" :title="isOrderingForSelf ? '我的收藏' : '他的收藏'" width="800px">
      <div v-if="favoriteDishes.length === 0" style="text-align: center; padding: 40px; color: #999;">
        暂无收藏菜品
      </div>
      <div v-else>
        <el-table :data="favoriteDishes" style="width: 100%">
          <el-table-column prop="dish_id" label="ID" width="80" />
          <el-table-column label="菜品名称">
            <template #default="scope">
              <span>{{ scope.row.name }}</span>
              <el-tag v-for="(tag, index) in scope.row.tags" :key="index" size="small" style="margin-left: 8px;">
                {{ tag }}
              </el-tag>
            </template>
          </el-table-column>
          <el-table-column label="价格" width="120">
            <template #default="scope">
              <div>
                <span v-if="discount < 1" style="text-decoration: line-through; color: #999;">
                  ¥{{ scope.row.price.toFixed(2) }}
                </span>
                <span v-else>¥{{ scope.row.price.toFixed(2) }}</span>
                <span v-if="discount < 1" style="margin-left: 10px; color: #f56c6c;">
                  ¥{{ (scope.row.price * discount).toFixed(2) }}
                </span>
              </div>
            </template>
          </el-table-column>
          <el-table-column label="数量" width="150">
            <template #default="scope">
              <el-input-number
                v-model="scope.row.quantity"
                :min="0"
                :max="10"
              />
            </template>
          </el-table-column>
        </el-table>
      </div>
      <template #footer>
        <span class="dialog-footer">
          <el-button @click="favoriteDialogVisible = false">取消</el-button>
          <el-button type="primary" @click="addToOrderFromFavorites">加入订单</el-button>
        </span>
      </template>
    </el-dialog>

  <!-- ⭐ 下单按钮 -->
    <el-button
      type="primary"
      style="margin-top: 20px"
      @click="submitOrder"
    >
      提交订单
    </el-button>

  </div>
</template>

<script setup>
import { ref, onMounted, computed } from 'vue'
import axios from 'axios'
import { useRoute } from 'vue-router'
import { ElMessage } from 'element-plus'
import { Star, StarFilled } from '@element-plus/icons-vue'

const route = useRoute()
const dishes = ref([])
const familyMembers = ref([])
const selectedOrderForUserId = ref(0)
const selectedUserAge = ref(0)
const selectedMealType = ref('午餐')
const nutritionDialogVisible = ref(false)
const selectedDish = ref(null)
const user = JSON.parse(localStorage.getItem('user'))

// 收藏相关
const favoriteDialogVisible = ref(false)
const favoriteDishes = ref([])

// 最近订单
const recentOrders = ref([])
const selectedOrder = ref(null)
const selectedOrderItems = ref([])
const missingDishes = ref([])
// 已选菜品
const selectedDishes = ref([])

// 推荐菜品
const recommendedDishes = ref([])

// 计算是否为自己点餐
const isOrderingForSelf = computed(() => {
  return selectedOrderForUserId.value === user.user_id
})

// 计算折扣
const discount = computed(() => {
  const age = selectedUserAge.value
  if (age >= 60 && age <= 69) {
    return 0.9 // 9折
  } else if (age >= 70 && age <= 79) {
    return 0.7 // 7折
  } else if (age >= 80 && age <= 89) {
    return 0.5 // 5折
  } else if (age >= 90) {
    return 0.0 // 免费
  }
  return 1.0 // 无折扣
})

// 折扣文本
const discountText = computed(() => {
  const age = selectedUserAge.value
  if (age >= 60 && age <= 69) {
    return '9折优惠'
  } else if (age >= 70 && age <= 79) {
    return '7折优惠'
  } else if (age >= 80 && age <= 89) {
    return '5折优惠'
  } else if (age >= 90) {
    return '免费'
  }
  return '无优惠'
})

// 获取用户年龄
const getUserAge = async (user_id) => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/userCenter', {
      params: {
        user_id: user_id
      }
    })

    if (res.data.code === 0) {
      selectedUserAge.value = res.data.data.age
    }
  } catch (error) {
    console.error('获取用户年龄失败:', error)
  }
}

// 切换用餐者
const handleOrderForUserChange = async (user_id) => {
  await getUserAge(user_id)
  await getRecentOrder()
  await checkFavoritesStatus()
  await getRecommendedDishes()
}

// 获取最近订单信息
const getRecentOrder = async () => {
  const canteen_id = Number(route.query.canteen_id)
  const order_for_user_id = Number(selectedOrderForUserId.value || user.user_id)
  
  try {
    const res = await axios.get('http://192.168.56.100:8080/recentOrders', {
      params: {
        user_id: user.user_id,
        order_for_user_id: order_for_user_id,
        canteen_id: canteen_id,
        limit: 4
      }
    })
    
    if (res.data.code === 0 && res.data.data) {
      recentOrders.value = res.data.data || []
      if (recentOrders.value.length > 0) {
        selectOrder(recentOrders.value[0])
      }
    } else {
      recentOrders.value = []
      selectedOrder.value = null
      selectedOrderItems.value = []
      missingDishes.value = []
    }
  } catch (error) {
    console.error('获取最近订单失败:', error)
    recentOrders.value = []
    selectedOrder.value = null
    selectedOrderItems.value = []
    missingDishes.value = []
  }
}

// 选择订单
const selectOrder = (order) => {
  selectedOrder.value = order
  selectedOrderItems.value = order.items || []
  
  // 检查哪些菜品在当前餐单中不存在
  const availableDishes = new Set(dishes.value.map(dish => dish.id))
  missingDishes.value = selectedOrderItems.value
    .filter(item => !availableDishes.has(item.dish_id))
    .map(item => item.dish_name)
}

// 添加到订单
const addToOrder = () => {
  if (!selectedOrder.value) return
  
  const availableDishes = new Set(dishes.value.map(dish => dish.id))
  
  // 添加存在的菜品到已选菜品
  selectedOrderItems.value.forEach(item => {
    if (availableDishes.has(item.dish_id)) {
      // 找到菜单中对应的菜品，增加数量
      const dish = dishes.value.find(d => d.id === item.dish_id)
      if (dish) {
        dish.quantity += item.quantity
        ElMessage.success(`已添加 ${dish.name} × ${item.quantity} 到订单`)
      }
    }
  })
  
  // 重新检查缺失的菜品
  missingDishes.value = selectedOrderItems.value
    .filter(item => !availableDishes.has(item.dish_id))
    .map(item => item.dish_name)
}

// 获取推荐菜品
const getRecommendedDishes = async () => {
  const canteen_id = Number(route.query.canteen_id)
  const order_for_user_id = Number(selectedOrderForUserId.value || user.user_id)
  
  try {
    const res = await axios.get('http://192.168.56.100:8080/recommendedDishes', {
      params: {
        user_id: order_for_user_id,
        canteen_id: canteen_id,
        meal_type: selectedMealType.value
      }
    })
    
    if (res.data.code === 0) {
      // 后端返回的数据结构是 { recommended_dishes: [...] }
      const dishesData = res.data.data.recommended_dishes || []
      recommendedDishes.value = dishesData.map(dish => ({
        dishId: dish.dish_id,
        dishName: dish.dish_name,
        price: dish.price,
        tags: dish.tags,
        recommendationScore: dish.recommendation_score,
        recommendationReason: dish.recommendation_reason,
        quantity: 0,
        ...getRecommendationType(dish.recommendation_reason)
      }))
    }
  } catch (error) {
    console.error('获取推荐菜品失败:', error)
  }
}

// 判断推荐类型
const getRecommendationType = (reason) => {
  if (reason.includes('疾病推荐') || reason.includes('健康')) {
    return { type: 'health', color: '#67c23a', icon: '❤', bgColor: '#f0f9eb' }
  } else if (reason.includes('热门')) {
    return { type: 'popular', color: '#e6a23c', icon: '🔥', bgColor: '#fdf6ec' }
  } else if (reason.includes('偏好')) {
    return { type: 'preference', color: '#409eff', icon: '👍', bgColor: '#ecf5ff' }
  }
  return { type: 'default', color: '#909399', icon: '⭐', bgColor: '#f5f5f5' }
}

// 从推荐菜品添加到订单
const addFromRecommendation = (dish) => {
  const targetDish = dishes.value.find(d => d.id === dish.dishId)
  if (targetDish) {
    targetDish.quantity += 1
    ElMessage.success(`已添加 ${dish.dishName}`)
  } else {
    ElMessage.warning('该菜品不在当前餐单中')
  }
}

// 显示营养信息
const showNutritionInfo = (dish) => {
  selectedDish.value = dish
  nutritionDialogVisible.value = true
}

// 获取菜单
const getMenu = async () => {
  const canteen_id = Number(route.query.canteen_id)

  const res = await axios.get('http://192.168.56.100:8080/menu', {
    params: {
      canteen_id: canteen_id,
      meal_type: selectedMealType.value
    }
  })

  // 初始化 quantity
  dishes.value = res.data.data.map(d => ({
    ...d,
    quantity: 0,
    isFavorite: false
  }))

  // 获取收藏状态
  await checkFavoritesStatus()
}

// 检查收藏状态
const checkFavoritesStatus = async () => {
  const canteen_id = Number(route.query.canteen_id)
  const user_id = selectedOrderForUserId.value

  for (const dish of dishes.value) {
    try {
      const res = await axios.get('http://192.168.56.100:8080/checkFavorite', {
        params: {
          user_id: user_id,
          dish_id: dish.id
        }
      })
      if (res.data.code === 0) {
        dish.isFavorite = res.data.data.is_favorite
      }
    } catch (error) {
      console.error('检查收藏状态失败:', error)
    }
  }
}

// 切换收藏状态
const toggleFavorite = async (dish) => {
  const user_id = selectedOrderForUserId.value

  try {
    if (dish.isFavorite) {
      const res = await axios.post('http://192.168.56.100:8080/removeFavorite', {
        user_id: user_id,
        dish_id: dish.id
      })
      if (res.data.code === 0) {
        dish.isFavorite = false
        ElMessage.success('已取消收藏')
      } else {
        ElMessage.error(res.data.message || '取消收藏失败')
      }
    } else {
      const res = await axios.post('http://192.168.56.100:8080/addFavorite', {
        user_id: user_id,
        dish_id: dish.id
      })
      if (res.data.code === 0) {
        dish.isFavorite = true
        ElMessage.success('已添加收藏')
      } else {
        ElMessage.error(res.data.message || '添加收藏失败')
      }
    }
  } catch (error) {
    console.error('切换收藏失败:', error)
    ElMessage.error('操作失败')
  }
}

// 打开收藏弹窗
const openFavoriteDialog = async () => {
  const canteen_id = Number(route.query.canteen_id)
  const user_id = selectedOrderForUserId.value

  try {
    const res = await axios.get('http://192.168.56.100:8080/getFavorites', {
      params: {
        user_id: user_id,
        canteen_id: canteen_id
      }
    })

    if (res.data.code === 0) {
      favoriteDishes.value = res.data.data.map(d => ({
        ...d,
        quantity: 0
      }))
      favoriteDialogVisible.value = true
    } else {
      ElMessage.error(res.data.message || '获取收藏失败')
    }
  } catch (error) {
    console.error('获取收藏失败:', error)
    ElMessage.error('获取收藏失败')
  }
}

// 从收藏中添加到订单
const addToOrderFromFavorites = () => {
  favoriteDishes.value.forEach(favDish => {
    if (favDish.quantity > 0) {
      const dish = dishes.value.find(d => d.id === favDish.dish_id)
      if (dish) {
        dish.quantity = favDish.quantity
      }
    }
  })
  favoriteDialogVisible.value = false
  ElMessage.success('已添加到订单')
}

// 初始化
onMounted(async () => {
  const canteen_id = Number(route.query.canteen_id)

  const orderTargetRes = await axios.get('http://192.168.56.100:8080/orderTargets', {
    params: {
      user_id: user.user_id
    }
  })

  if (orderTargetRes.data.code === 0) {
    familyMembers.value = orderTargetRes.data.data
    if (familyMembers.value.length > 0) {
      selectedOrderForUserId.value = familyMembers.value[0].user_id
      await getUserAge(selectedOrderForUserId.value)
    } else {
      selectedOrderForUserId.value = user.user_id
      await getUserAge(user.user_id)
    }
  } else {
    ElMessage.warning('家庭成员加载失败，默认给自己点餐')
    selectedOrderForUserId.value = user.user_id
    await getUserAge(user.user_id)
  }

  // 获取菜单
  await getMenu()
  
  // 获取最近订单
  await getRecentOrder()
  
  // 获取推荐菜品
  await getRecommendedDishes()
})

// ⭐ 提交订单
const submitOrder = async () => {
  const canteen_id = route.query.canteen_id

  // 过滤选中的菜
  const items = dishes.value
    .filter(d => d.quantity > 0)
    .map(d => ({
      dish_id: d.id,
      quantity: d.quantity
    }))

  if (items.length === 0) {
    ElMessage.warning('请选择菜品')
    return
  }

  try {
    const res = await axios.post(
      'http://192.168.56.100:8080/placeOrder',
      {
        user_id: user.user_id,
        order_for_user_id: Number(selectedOrderForUserId.value || user.user_id),
        canteen_id: Number(canteen_id),
        items: items
      }
    )

    if (res.data.code === 0) {
      ElMessage.success('下单成功 🎉')

      // ⭐ 清空数量
      dishes.value.forEach(d => d.quantity = 0)
    } else {
      ElMessage.error(res.data.message || '下单失败')
    }

  } catch (error) {
    ElMessage.error('请求失败')
  }
}
</script>

<style scoped>
.recent-orders-container {
  display: flex;
  gap: 15px;
  overflow-x: auto;
  padding-bottom: 10px;
}

.recent-order-card {
  min-width: 280px;
  max-width: 320px;
  cursor: pointer;
  transition: all 0.3s;
  border: 2px solid transparent;
}

.recent-order-card:hover {
  transform: translateY(-3px);
  box-shadow: 0 6px 12px rgba(0, 0, 0, 0.1);
}

.recent-order-card.active {
  border-color: #409eff;
  background-color: #f0f7ff;
}

.order-card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 10px;
}

.order-time {
  font-size: 14px;
  color: #666;
}

.order-dishes {
  display: flex;
  flex-wrap: wrap;
  gap: 6px;
  margin-bottom: 10px;
}

.dish-tag {
  background-color: #ecf5ff;
  color: #409eff;
  padding: 2px 8px;
  border-radius: 4px;
  font-size: 12px;
}

.more-dishes {
  color: #909399;
  font-size: 12px;
  padding: 2px 8px;
}

.order-price {
  display: flex;
  justify-content: flex-end;
  align-items: center;
  padding-top: 8px;
  border-top: 1px solid #eee;
}

.price-label {
  font-size: 12px;
  color: #666;
  margin-right: 5px;
}

.price-value {
  font-size: 16px;
  font-weight: 600;
  color: #f56c6c;
}
</style>
