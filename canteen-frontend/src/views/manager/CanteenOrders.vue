<template>
  <div class="canteen-orders">
    <h2>食堂订单管理</h2>
    
    <!-- 取消申请列表 -->
    <el-card v-if="cancelApplies.length > 0" class="cancel-applies-card">
      <template #header>
        <div class="card-header">
          <span>申请取消的订单</span>
        </div>
      </template>
      <el-table :data="cancelApplies" style="width: 100%">
        <el-table-column prop="order_id" label="订单ID" width="100" />
        <el-table-column prop="order_for_user_name" label="用餐者" width="150" />
        <el-table-column prop="order_time" label="订单时间" width="200" />
        <el-table-column prop="total_price" label="总价" width="100">
          <template #default="scope">
            ¥{{ scope.row.total_price.toFixed(2) }}
          </template>
        </el-table-column>
        <el-table-column prop="cancel_time" label="申请时间" width="200" />
        <el-table-column prop="cancel_reason" label="取消原因" />
        <el-table-column label="操作" width="200">
          <template #default="scope">
            <el-button
              type="success"
              size="small"
              @click="handleCancelApply(scope.row.cancel_id, 1)"
            >
              同意
            </el-button>
            <el-button
              type="danger"
              size="small"
              @click="openRejectDialog(scope.row)"
            >
              拒绝
            </el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>
    
    <!-- 订单状态标签页 -->
    <el-tabs v-model="activeStatusTab" @tab-click="handleStatusTabChange" style="margin-bottom: 20px">
      <el-tab-pane label="未完成" name="0" />
      <el-tab-pane label="已完成" name="1" />
      <el-tab-pane label="已取消" name="2" />
    </el-tabs>

    <!-- 普通订单列表 -->
    <el-table :data="filteredOrders" style="width: 100%">
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
          <el-tag :type="getTagType(scope.row.status)">
            {{ getStatusText(scope.row.status) }}
          </el-tag>
        </template>
      </el-table-column>
      <el-table-column label="操作" width="250">
        <template #default="scope">
          <el-button
            type="primary"
            size="small"
            @click="viewOrderDetails(scope.row)"
          >
            详细信息
          </el-button>
          <el-button
            v-if="scope.row.status === 0"
            type="success"
            size="small"
            @click="updateOrderStatus(scope.row.order_id, 1)"
          >
            标记为已完成
          </el-button>
        </template>
      </el-table-column>
    </el-table>

    <!-- 订单详情弹窗 -->
    <el-dialog
      v-model="orderDetailsDialogVisible"
      title="订单详细信息"
      width="600px"
    >
      <div v-if="currentOrderDetails">
        <el-form label-width="100px">
          <el-form-item label="订单ID">
            <span>{{ currentOrderDetails.order_id }}</span>
          </el-form-item>
          <el-form-item label="用餐者">
            <span>{{ currentOrderDetails.order_for_user_name }}</span>
          </el-form-item>
          <el-form-item label="订单时间">
            <span>{{ currentOrderDetails.order_time }}</span>
          </el-form-item>
          <el-form-item label="总价">
            <span>¥{{ currentOrderDetails.total_price.toFixed(2) }}</span>
          </el-form-item>
          <el-form-item label="状态">
            <el-tag :type="getTagType(currentOrderDetails.status)">
              {{ getStatusText(currentOrderDetails.status) }}
            </el-tag>
          </el-form-item>
          <el-form-item label="菜品详情">
            <el-table :data="currentOrderDetails.items" style="width: 100%">
              <el-table-column prop="dish_name" label="菜品" width="200" />
              <el-table-column prop="unit_price" label="单价" width="100" />
              <el-table-column prop="quantity" label="数量" width="100" />
              <el-table-column prop="subtotal" label="小计" width="100" />
            </el-table>
          </el-form-item>
        </el-form>
      </div>
      <template #footer>
        <span class="dialog-footer">
          <el-button @click="orderDetailsDialogVisible = false">关闭</el-button>
        </span>
      </template>
    </el-dialog>
    
    <!-- 拒绝取消申请弹窗 -->
    <el-dialog
      v-model="rejectDialogVisible"
      title="拒绝取消申请"
      width="400px"
    >
      <el-form :model="rejectForm" label-width="80px">
        <el-form-item label="订单ID">
          <span>{{ currentCancelApply?.order_id }}</span>
        </el-form-item>
        <el-form-item label="拒绝原因">
          <el-input
            v-model="rejectForm.reject_reason"
            type="textarea"
            :rows="4"
            placeholder="请输入拒绝原因"
            maxlength="200"
            show-word-limit
          />
        </el-form-item>
      </el-form>
      <template #footer>
        <span class="dialog-footer">
          <el-button @click="rejectDialogVisible = false">取消</el-button>
          <el-button type="primary" @click="submitRejectApply">提交</el-button>
        </span>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, onMounted, computed } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

const orders = ref([])
const cancelApplies = ref([])
const rejectDialogVisible = ref(false)
const orderDetailsDialogVisible = ref(false)
const currentCancelApply = ref(null)
const currentOrderDetails = ref(null)
const activeStatusTab = ref('0') // 默认显示未完成订单
const rejectForm = ref({
  reject_reason: ''
})

// 筛选订单
const filteredOrders = computed(() => {
  if (activeStatusTab.value === '') {
    return orders.value
  }
  return orders.value.filter(order => order.status === Number(activeStatusTab.value))
})

// 处理状态标签切换
const handleStatusTabChange = () => {
  // 状态标签切换时无需额外操作
}

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

// 获取取消申请列表
const getCancelApplies = async () => {
  try {
    const canteen_id = await getCanteenId()
    if (!canteen_id) return

    const res = await axios.get('http://192.168.56.100:8080/getCancelApplies', {
      params: { canteen_id }
    })
    // 只显示状态为0（申请取消）的记录
    cancelApplies.value = res.data.data.filter(apply => apply.status === 0)
  } catch (error) {
    console.error('获取取消申请列表失败:', error)
    ElMessage.error('获取取消申请列表失败')
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

// 处理取消申请
const handleCancelApply = async (cancel_id, status) => {
  try {
    const res = await axios.post('http://192.168.56.100:8080/handleCancelApply', {
      cancel_id,
      status
    })
    if (res.data.code === 0) {
      ElMessage.success(status === 1 ? '同意取消成功' : '拒绝取消成功')
      getCancelApplies()
      getOrders()
    } else {
      ElMessage.error('处理取消申请失败')
    }
  } catch (error) {
    console.error('处理取消申请失败:', error)
    ElMessage.error('处理取消申请失败')
  }
}

// 打开拒绝弹窗
const openRejectDialog = (apply) => {
  currentCancelApply.value = apply
  rejectForm.value.reject_reason = ''
  rejectDialogVisible.value = true
}

// 提交拒绝申请
const submitRejectApply = async () => {
  if (!currentCancelApply.value) {
    return
  }
  if (!rejectForm.value.reject_reason.trim()) {
    ElMessage.warning('请输入拒绝原因')
    return
  }

  try {
    const res = await axios.post('http://192.168.56.100:8080/handleCancelApply', {
      cancel_id: currentCancelApply.value.cancel_id,
      status: 2,
      reject_reason: rejectForm.value.reject_reason
    })
    if (res.data.code === 0) {
      ElMessage.success('拒绝取消成功')
      rejectDialogVisible.value = false
      getCancelApplies()
      getOrders()
    } else {
      ElMessage.error('拒绝取消失败')
    }
  } catch (error) {
    console.error('拒绝取消失败:', error)
    ElMessage.error('拒绝取消失败')
  }
}

// 查看订单详情
const viewOrderDetails = async (order) => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/orderDetails', {
      params: {
        order_id: order.order_id
      }
    })
    if (res.data.code === 0) {
      currentOrderDetails.value = {
        ...order,
        items: res.data.data
      }
      orderDetailsDialogVisible.value = true
    } else {
      ElMessage.error('获取订单详情失败')
    }
  } catch (error) {
    console.error('获取订单详情失败:', error)
    ElMessage.error('获取订单详情失败')
  }
}

// 根据状态获取标签类型
const getTagType = (status) => {
  switch (status) {
    case 0:
      return 'warning'
    case 1:
      return 'success'
    case 2:
      return 'danger'
    default:
      return ''
  }
}

// 根据状态获取状态文本
const getStatusText = (status) => {
  switch (status) {
    case 0:
      return '未完成'
    case 1:
      return '已完成'
    case 2:
      return '已取消'
    default:
      return '未知'
  }
}

// 页面挂载时获取订单列表和取消申请列表
onMounted(() => {
  getOrders()
  getCancelApplies()
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

.cancel-applies-card {
  margin-bottom: 20px;
  border-left: 4px solid #ff9900;
}

.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.dialog-footer {
  display: flex;
  justify-content: flex-end;
  gap: 10px;
}
</style>