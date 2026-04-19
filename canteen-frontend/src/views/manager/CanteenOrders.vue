<template>
  <div class="canteen-orders">
    <h2>食堂订单管理</h2>
    <el-table :data="orders" style="width: 100%">
      <el-table-column prop="order_id" label="订单ID" width="100" />
      <el-table-column prop="order_for_user_name" label="用餐者" width="150" />
      <el-table-column prop="order_time" label="订单时间" width="200" />
      <el-table-column prop="total_price" label="总价" width="100">
        <template #default="scope">
          ¥{{ scope.row.total_price.toFixed(2) }}
        </template>
      </el-table-column>
      <el-table-column prop="status" label="状态" width="120">
        <template #default="scope">
          <el-tag :type="scope.row.status === 0 ? 'warning' : 'success'">
            {{ scope.row.status === 0 ? '未完成' : '已完成' }}
          </el-tag>
        </template>
      </el-table-column>
      <el-table-column label="操作" width="150">
        <template #default="scope">
          <el-button
            v-if="scope.row.status === 0"
            type="success"
            size="small"
            @click="updateOrderStatus(scope.row.order_id, 1)"
          >
            标记为已完成
          </el-button>
          <span v-else>已完成</span>
        </template>
      </el-table-column>
    </el-table>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

const orders = ref([])

// 获取食堂ID
const getCanteenId = async () => {
  try {
    const user = JSON.parse(localStorage.getItem('user'))
    const user_id = user?.user_id
    if (!user_id) {
      ElMessage.error('未登录或用户信息不存在')
      return null
    }
    const res = await axios.get('http://192.168.56.100:8080/myCanteen', {
      params: { user_id }
    })
    console.log('获取食堂ID成功:', res.data.data.id)
    return res.data.data.id
  } catch (error) {
    console.error('获取食堂ID失败:', error)
    ElMessage.error('获取食堂信息失败')
    return null
  }
}

// 获取订单列表
const getOrders = async () => {
  try {
    const canteen_id = await getCanteenId()
    if (!canteen_id) return

    const res = await axios.get('http://192.168.56.100:8080/canteenOrders', {
      params: { canteen_id }
    })
    orders.value = res.data.data
  } catch (error) {
    console.error('获取订单列表失败:', error)
    ElMessage.error('获取订单列表失败')
  }
}

// 更新订单状态
const updateOrderStatus = async (order_id, status) => {
  try {
    const res = await axios.post('http://192.168.56.100:8080/updateOrderStatus', {
      order_id,
      status
    })
    if (res.data.code === 0) {
      ElMessage.success('订单状态更新成功')
      getOrders()
    } else {
      ElMessage.error('订单状态更新失败')
    }
  } catch (error) {
    console.error('更新订单状态失败:', error)
    ElMessage.error('更新订单状态失败')
  }
}

// 页面挂载时获取订单列表
onMounted(() => {
  getOrders()
})
</script>

<style scoped>
.canteen-orders {
  padding: 20px;
}

h2 {
  margin-bottom: 20px;
  color: #303133;
}
</style>