<template>
  <div>

    <h2>我的订单</h2>

    <el-table ref="tableRef" :data="orders" style="width: 100%">

      <!-- 展开行 -->
      <el-table-column type="expand">
        <template #default="scope">

          <el-table :data="scope.row.items" border>
            <el-table-column prop="dish_name" label="菜品" />
            <el-table-column prop="quantity" label="数量" />
            <el-table-column prop="price" label="价格" />
          </el-table>

        </template>
      </el-table-column>

      <el-table-column prop="order_id" label="订单ID" />
      <el-table-column prop="order_for_user_id" label="为谁购买" />
      <el-table-column prop="canteen_name" label="食堂" />
      <el-table-column prop="total_price" label="总价" />
      <el-table-column prop="create_time" label="时间" />

      <el-table-column label="操作">
        <template #default="scope">
        <el-button @click="handleExpand(scope.row)">
      查看详情
    </el-button>
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
const tableRef = ref()

const testClick = (scope) => {
  console.log('scope:', scope)
}

const handleExpand = async (row) => {
  await loadDetails(row)

  // ⭐ 自动展开当前行
  tableRef.value.toggleRowExpansion(row, true)
  }

// ⭐ 获取订单列表
const loadOrders = async () => {
  try {
    const user = JSON.parse(localStorage.getItem('user'))

    const res = await axios.get('http://192.168.56.100:8080/orders', {
      params: {
        user_id: user.user_id
      }
    })

    if (res.data.code === 0) {
      orders.value = res.data.data

      // 初始化 items（防止 undefined）
      orders.value.forEach(o => {
        o.items = []
        o.loaded = false
      })

    } else {
      ElMessage.error('获取订单失败')
    }

  } catch (e) {
    ElMessage.error('请求失败')
  }
}

// ⭐ 加载订单详情（点击触发🔥）
const loadDetails = async (order) => {
  try {
    // 防止重复请求
    if (order.loaded) return

    const user = JSON.parse(localStorage.getItem('user'))  // ⭐补上
    const res = await axios.get('http://192.168.56.100:8080/order_details', {
      params: {
        user_id: user.user_id,
        order_id: order.order_id
      }
    })
console.log('返回数据:', res.data)
console.log('items:', res.data.data)
    if (res.data.code === 0) {
      order.items = res.data.data
      order.loaded = true
    } else {
      ElMessage.error('加载详情失败')
    }

  } catch (e) {
    ElMessage.error('请求失败')
  }
}

onMounted(() => {
  loadOrders()
})
</script>