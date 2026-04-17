<template>
  <div>
    <h2>菜单列表</h2>

    <el-form inline style="margin-bottom: 16px">
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

    <el-table :data="dishes" style="width: 100%">
      <el-table-column prop="id" label="ID" width="100" />
      <el-table-column prop="name" label="菜品名称" />
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

    </el-table>

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
const user = JSON.parse(localStorage.getItem('user'))

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
}

// 获取菜单
onMounted(async () => {
  const canteen_id = Number(route.query.canteen_id)

  const [menuRes, orderTargetRes] = await Promise.all([
    axios.get('http://192.168.56.100:8080/menu', {
      params: {
        canteen_id: canteen_id,
        date: getToday()
      }
    }),
    axios.get('http://192.168.56.100:8080/orderTargets', {
      params: {
        user_id: user.user_id
      }
    })
  ])

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

  // 初始化 quantity
  dishes.value = menuRes.data.data.map(d => ({
    ...d,
    quantity: 0
  }))
})

const getToday = () => {
  const now = new Date()

  const year = now.getFullYear()
  const month = String(now.getMonth() + 1).padStart(2, '0')
  const day = String(now.getDate()).padStart(2, '0')

  return `${year}-${month}-${day}`
}
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
