<template>
  <div>

    <h2>我的订单</h2>

    <el-tabs v-model="activeStatusTab" @tab-click="handleStatusTabChange" style="margin-bottom: 20px">
      <el-tab-pane label="未完成" name="0" />
      <el-tab-pane label="已完成" name="1" />
      <el-tab-pane label="已取消" name="2" />
    </el-tabs>

    <el-tabs v-if="activeStatusTab === '1'" v-model="activeRatingTab" @tab-click="handleRatingTabChange" style="margin-bottom: 20px">
      <el-tab-pane label="全部" name="all" />
      <el-tab-pane label="未评价" name="unrated" />
      <el-tab-pane label="已评价" name="rated" />
    </el-tabs>

    <el-table ref="tableRef" :data="filteredOrders" style="width: 100%">

      <!-- 展开行 -->
      <el-table-column type="expand">
        <template #default="scope">

          <el-table :data="scope.row.items" border align="center">
            <el-table-column prop="dish_name" label="菜品" align="center" />
            <el-table-column prop="unit_price" label="原价" align="center" />
            <el-table-column prop="discount_price" label="优惠价" align="center" />
            <el-table-column prop="quantity" label="数量" align="center" />
            <el-table-column prop="subtotal" label="小计" align="center" />
          </el-table>

        </template>
      </el-table-column>

      <el-table-column prop="order_id" label="订单ID" />
      <el-table-column prop="order_for_user_name" label="为谁购买" />
      <el-table-column prop="canteen_name" label="食堂" />
      <el-table-column prop="original_total" label="原价" />
      <el-table-column prop="discount_rate" label="优惠率" />
      <el-table-column prop="total_price" label="实付" />
      <el-table-column prop="saved_amount" label="优惠" />
      <el-table-column prop="create_time" label="时间" />
      <el-table-column prop="status" label="状态" width="100">
        <template #default="scope">
          <el-tag :type="scope.row.status === 0 ? 'warning' : scope.row.status === 1 ? 'success' : 'danger'">
            {{ scope.row.status === 0 ? '未完成' : scope.row.status === 1 ? '已完成' : '已取消' }}
          </el-tag>
        </template>
      </el-table-column>

      <el-table-column label="操作" width="400">
        <template #default="scope">
          <el-button @click="handleExpand(scope.row)">查看详情</el-button>
          
          <!-- 未完成订单 -->
          <el-button
            v-if="scope.row.status === 0"
            type="warning"
            @click="openCancelDialog(scope.row)"
          >
            取消订单
          </el-button>
          
          <!-- 已取消订单 -->
          <template v-else-if="scope.row.status === 2">
            <el-button type="info" @click="viewCancelReason(scope.row)">查看取消原因</el-button>
          </template>
          
          <!-- 已完成订单 -->
          <template v-else-if="scope.row.status === 1">
            <el-button
              v-if="!scope.row.has_rating"
              type="success"
              @click="openRatingDialog(scope.row)"
            >
              评价
            </el-button>
            <el-button
              v-else
              type="info"
              @click="viewMyRating(scope.row)"
            >
              查看评价
            </el-button>
          </template>
        </template>
      </el-table-column>

    </el-table>

    <!-- 取消订单弹窗 -->
    <el-dialog
      v-model="cancelDialogVisible"
      title="取消订单"
      width="400px"
    >
      <el-form :model="cancelForm" label-width="80px">
        <el-form-item label="订单ID">
          <span>{{ currentOrder?.order_id }}</span>
        </el-form-item>
        <el-form-item label="取消原因">
          <el-input
            v-model="cancelForm.cancel_reason"
            type="textarea"
            :rows="4"
            placeholder="请输入取消原因"
            maxlength="200"
            show-word-limit
          />
        </el-form-item>
      </el-form>
      <template #footer>
        <span class="dialog-footer">
          <el-button @click="cancelDialogVisible = false">取消</el-button>
          <el-button type="primary" @click="submitCancelApply">提交</el-button>
        </span>
      </template>
    </el-dialog>

    <!-- 查看取消原因弹窗 -->
    <el-dialog
      v-model="cancelReasonDialogVisible"
      title="取消原因"
      width="400px"
    >
      <el-form label-width="80px">
        <el-form-item label="订单ID">
          <span>{{ currentOrder?.order_id }}</span>
        </el-form-item>
        <el-form-item label="取消原因">
          <span>{{ currentOrder?.cancel_reason || '-' }}</span>
        </el-form-item>
        <el-form-item label="取消时间">
          <span>{{ currentOrder?.cancel_time || '-' }}</span>
        </el-form-item>
      </el-form>
      <template #footer>
        <span class="dialog-footer">
          <el-button @click="cancelReasonDialogVisible = false">关闭</el-button>
        </span>
      </template>
    </el-dialog>

    <el-dialog v-model="ratingDialogVisible" :title="ratingDialogTitle" width="520px">
      <el-form label-width="90px">
        <el-form-item label="订单ID">
          <span>{{ currentOrder?.order_id }}</span>
        </el-form-item>
        <el-form-item label="评分">
          <el-rate v-model="ratingForm.score" :max="5" show-score :disabled="ratingReadonly" />
        </el-form-item>
        <el-form-item label="评价内容">
          <el-input
            v-model="ratingForm.comment"
            type="textarea"
            :rows="4"
            maxlength="300"
            show-word-limit
            :disabled="ratingReadonly"
            placeholder="请输入评价内容"
          />
        </el-form-item>
        <el-form-item v-if="ratingReadonly" label="评价时间">
          <span>{{ currentOrder?.rating_time || '-' }}</span>
        </el-form-item>
      </el-form>

      <template #footer>
        <el-button @click="ratingDialogVisible = false">{{ ratingReadonly ? '关闭' : '取消' }}</el-button>
        <el-button v-if="!ratingReadonly" type="primary" @click="submitRating">提交评价</el-button>
      </template>
    </el-dialog>

  </div>
</template>

<script setup>
import { ref, onMounted, reactive, computed } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

const orders = ref([])
const tableRef = ref()
const ratingDialogVisible = ref(false)
const cancelDialogVisible = ref(false)
const cancelReasonDialogVisible = ref(false)
const ratingReadonly = ref(false)
const currentOrder = ref(null)
const activeStatusTab = ref('0') // 默认显示未完成订单
const activeRatingTab = ref('all')
const ratingForm = reactive({
  score: 0,
  comment: ''
})
const cancelForm = reactive({
  cancel_reason: ''
})

// 筛选订单
const filteredOrders = computed(() => {
  let result = orders.value
  
  // 按状态筛选
  if (activeStatusTab.value !== '') {
    result = result.filter(order => order.status === Number(activeStatusTab.value))
  }
  
  // 已完成订单按评价状态筛选
  if (activeStatusTab.value === '1') {
    if (activeRatingTab.value === 'unrated') {
      result = result.filter(order => !order.has_rating)
    } else if (activeRatingTab.value === 'rated') {
      result = result.filter(order => order.has_rating)
    }
  }
  
  return result
})

// 处理状态标签切换
const handleStatusTabChange = () => {
  // 当切换到已完成标签时，重置评价状态标签为全部
  if (activeStatusTab.value === '1') {
    activeRatingTab.value = 'all'
  }
}

// 处理评价状态标签切换
const handleRatingTabChange = () => {
  // 评价状态标签切换时无需额外操作
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
  ratingReadonly.value = false
  ratingForm.score = 0
  ratingForm.comment = ''
  ratingDialogVisible.value = true
}

const viewMyRating = (order) => {
  currentOrder.value = order
  ratingReadonly.value = true
  ratingForm.score = Number(order.rating_score || 0)
  ratingForm.comment = order.rating_comment || ''
  ratingDialogVisible.value = true
}

const ratingDialogTitle = computed(() => (ratingReadonly.value ? '我的评价记录' : '订单评价'))

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
      currentOrder.value.has_rating = true
      currentOrder.value.rating_score = ratingForm.score
      currentOrder.value.rating_comment = ratingForm.comment || ''
      currentOrder.value.rating_time = new Date().toLocaleString()
      ratingDialogVisible.value = false
    } else {
      ElMessage.error(res.data.message || '评价失败')
    }
  } catch (e) {
    ElMessage.error('评价失败，可能该订单已评价')
  }
}

const openCancelDialog = (order) => {
  currentOrder.value = order
  cancelForm.cancel_reason = ''
  cancelDialogVisible.value = true
}

const submitCancelApply = async () => {
  if (!currentOrder.value) {
    return
  }
  if (!cancelForm.cancel_reason.trim()) {
    ElMessage.warning('请输入取消原因')
    return
  }

  try {
    const res = await axios.post('http://192.168.56.100:8080/createCancelApply', {
      order_id: currentOrder.value.order_id,
      cancel_reason: cancelForm.cancel_reason
    })

    if (res.data.code === 0) {
      ElMessage.success('取消申请提交成功')
      cancelDialogVisible.value = false
      // 刷新订单列表
      loadOrders()
    } else {
      ElMessage.error(res.data.message || '取消申请提交失败')
    }
  } catch (e) {
    ElMessage.error('取消申请提交失败')
  }
}

// 查看取消原因
const viewCancelReason = async (order) => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/getCancelApplyByOrderId', {
      params: {
        order_id: order.order_id
      }
    })
    
    if (res.data.code === 0 && res.data.data) {
      const cancelApply = res.data.data
      order.cancel_reason = cancelApply.cancel_reason
      order.cancel_time = cancelApply.cancel_time
    } else {
      order.cancel_reason = '暂无取消原因'
      order.cancel_time = '未知'
    }
    
    currentOrder.value = order
    cancelReasonDialogVisible.value = true
  } catch (e) {
    console.error('获取取消原因失败', e)
    order.cancel_reason = '获取失败'
    order.cancel_time = '未知'
    currentOrder.value = order
    cancelReasonDialogVisible.value = true
  }
}

onMounted(() => {
  loadOrders()
})
</script>
