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
    </el-form>

    <!-- 最近订单信息 -->
    <div v-if="showRecentOrder" style="margin-bottom: 20px; padding: 15px; border: 1px solid #e4e7ed; border-radius: 4px; background-color: #f9f9f9;">
      <h3 style="margin-top: 0; margin-bottom: 10px;">最近订单</h3>
      <div v-if="recentOrderItems.length > 0">
        <div style="margin-bottom: 10px;">
          <span>订单时间：{{ recentOrder.order_time }}</span>
        </div>
        <div style="margin-bottom: 10px;">
          <span>菜品：</span>
          <span v-for="(item, index) in recentOrderItems" :key="index">
            {{ item.dish_name }} × {{ item.quantity }}
            <span v-if="index < recentOrderItems.length - 1">，</span>
          </span>
        </div>
      </div>
      <div v-else>
        暂无最近的订单信息
      </div>
    </div>
    <div v-else style="margin-bottom: 20px; padding: 15px; border: 1px solid #e4e7ed; border-radius: 4px; background-color: #f9f9f9;">
      暂无最近的订单信息
    </div>

    <!-- 已选菜品 -->
    <div v-if="showRecentOrder && recentOrderItems.length > 0" style="margin-bottom: 20px; padding: 15px; border: 1px solid #e4e7ed; border-radius: 4px; background-color: #f9f9f9;">
      <h3 style="margin-top: 0; margin-bottom: 10px;">已选菜品</h3>
      <div v-if="selectedDishes.length > 0">
        <div style="margin-bottom: 10px;">
          <span>菜品：</span>
          <span v-for="(item, index) in selectedDishes" :key="index">
            {{ item.dish_name }} × {{ item.quantity }}
            <span v-if="index < selectedDishes.length - 1">，</span>
          </span>
        </div>
       <el-button type="primary" @click="reorder">再次点单</el-button>
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

const route = useRoute()
const dishes = ref([])
const familyMembers = ref([])
const selectedOrderForUserId = ref(0)
const selectedUserAge = ref(0)
const selectedMealType = ref('午餐')
const nutritionDialogVisible = ref(false)
const selectedDish = ref(null)
const user = JSON.parse(localStorage.getItem('user'))

// 最近订单
const recentOrder = ref(null)
const recentOrderItems = ref([])
const showRecentOrder = ref(false)
const outOfStockDishes = ref([])
// 已选菜品
const selectedDishes = ref([])

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
}

// 获取最近订单信息
const getRecentOrder = async () => {
  const canteen_id = Number(route.query.canteen_id)
  const order_for_user_id = Number(selectedOrderForUserId.value || user.user_id)
  
  try {
    const res = await axios.get('http://192.168.56.100:8080/recentOrder', {
      params: {
        user_id: user.user_id,
        order_for_user_id: order_for_user_id,
        canteen_id: canteen_id
      }
    })
    
    if (res.data.code === 0 && res.data.data) {
      recentOrder.value = res.data.data
      recentOrderItems.value = res.data.data.items || []
      showRecentOrder.value = true
      
      // 检查菜品状态
      await checkDishesStatus()
    } else {
      showRecentOrder.value = false
      recentOrder.value = null
      recentOrderItems.value = []
      outOfStockDishes.value = []
    }
  } catch (error) {
    console.error('获取最近订单失败:', error)
    showRecentOrder.value = false
    recentOrder.value = null
    recentOrderItems.value = []
    outOfStockDishes.value = []
  }
}

// 检查菜品状态
const checkDishesStatus = async () => {
  outOfStockDishes.value = []
  selectedDishes.value = []
  
  try {
    // 获取当前餐单中的菜品ID集合
    const availableDishIds = new Set(dishes.value.map(dish => dish.id))
    
    // 检查最近订单中的菜品是否在当前餐单中
    for (const item of recentOrderItems.value) {
      if (!availableDishIds.has(item.dish_id)) {
        // 查找菜品名称
        const dish = recentOrderItems.value.find(d => d.dish_id === item.dish_id)
        if (dish) {
          outOfStockDishes.value.push(dish.dish_name)
        }
      } else {
        // 餐单中存在的菜品，添加到已选菜品
        selectedDishes.value.push(item)
      }
    }
    
    // 如果有餐单中没有的菜品，显示提示
    if (outOfStockDishes.value.length > 0) {
      ElMessage.warning(`${outOfStockDishes.value.join('、')} 餐单中没有，已自动移除`)
    }
  } catch (error) {
    console.error('检查菜品状态失败:', error)
  }
}

// 再次点单
const reorder = async () => {
  if (!recentOrder.value) return
  
  const canteen_id = route.query.canteen_id
  
  // 过滤掉餐单中没有的菜品
  const availableDishes = new Set(dishes.value.map(dish => dish.id))
  const items = recentOrderItems.value
    .filter(item => availableDishes.has(item.dish_id))
    .map(item => ({
      dish_id: item.dish_id,
      quantity: item.quantity
    }))
  
  if (items.length === 0) {
    ElMessage.warning('最近订单中的菜品在当前餐单中都不存在')
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
    } else {
      ElMessage.error(res.data.message || '下单失败')
    }
  } catch (error) {
    ElMessage.error('请求失败')
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
    quantity: 0
  }))
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
