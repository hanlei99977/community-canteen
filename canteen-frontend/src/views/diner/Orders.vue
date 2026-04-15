<template>
  <div>

    <h2>我的订单</h2>

    <el-table ref="tableRef" :data="orders" style="width: 100%">

      <!-- 展开行 -->
      <el-table-column type="expand">
        <template #default="scope">

          <el-table :data="scope.row.items" border align="center">
            <el-table-column prop="dish_name" label="菜品" align="center" />
            <el-table-column prop="price" label="价格" align="center" />
            <el-table-column prop="quantity" label="数量" align="center" />
          </el-table>

        </template>
      </el-table-column>

      <el-table-column prop="order_id" label="订单ID" />
      <el-table-column prop="order_for_user_name" label="为谁购买" />
      <el-table-column prop="canteen_name" label="食堂" />
      <el-table-column prop="total_price" label="总价" />
      <el-table-column prop="create_time" label="时间" />

      <el-table-column label="操作" width="240">
        <template #default="scope">
          <el-button @click="handleExpand(scope.row)">查看详情</el-button>
          <el-button type="success" @click="openRatingDialog(scope.row)">评价</el-button>
        </template>
      </el-table-column>

    </el-table>

    <el-dialog v-model="ratingDialogVisible" title="订单评价" width="520px">
      <el-form label-width="90px">
        <el-form-item label="订单ID">
          <span>{{ currentOrder?.order_id }}</span>
        </el-form-item>
        <el-form-item label="评分">
          <el-rate v-model="ratingForm.score" :max="5" show-score />
        </el-form-item>
        <el-form-item label="评价内容">
          <el-input
            v-model="ratingForm.comment"
            type="textarea"
            :rows="4"
            maxlength="300"
            show-word-limit
            placeholder="请输入评价内容"
          />
        </el-form-item>
      </el-form>

      <template #footer>
        <el-button @click="ratingDialogVisible = false">取消</el-button>
        <el-button type="primary" @click="submitRating">提交评价</el-button>
      </template>
    </el-dialog>

  </div>
</template>

<script setup>
import { ref, onMounted, reactive } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

const orders = ref([])
const tableRef = ref()
const ratingDialogVisible = ref(false)
const currentOrder = ref(null)
const ratingForm = reactive({
  score: 0,
  comment: ''
})

const handleExpand = async (row) => {
  await loadDetails(row)

  // ⭐ 自动展开当前行
  tableRef.value.toggleRowExpansion(row, true)
}

// ⭐ 获取订单列表
const loadOrders = async () => {
  try {
    const user = JSON.parse(localStorage.getItem('user'))

    const res = await axios.get('http://192.168.56.100:8080/getOrders', {
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

const openRatingDialog = (order) => {
  currentOrder.value = order
  ratingForm.score = 0
  ratingForm.comment = ''
  ratingDialogVisible.value = true
}

const submitRating = async () => {
  if (!currentOrder.value) {
    return
  }
  if (ratingForm.score < 1 || ratingForm.score > 5) {
    ElMessage.warning('请给出1-5分评分')
    return
  }

  try {
    const user = JSON.parse(localStorage.getItem('user'))
    const res = await axios.post('http://192.168.56.100:8080/rating', {
      user_id: user.user_id,
      canteen_id: currentOrder.value.canteen_id,
      order_id: currentOrder.value.order_id,
      score: ratingForm.score,
      comment: ratingForm.comment || ''
    })

    if (res.data.code === 0) {
      ElMessage.success('评价成功')
      ratingDialogVisible.value = false
    } else {
      ElMessage.error(res.data.message || '评价失败')
    }
  } catch (e) {
    ElMessage.error('评价失败，可能该订单已评价')
  }
}

onMounted(() => {
  loadOrders()
})
</script>
