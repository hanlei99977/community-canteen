<template>
  <div class="canteen-manage">
    <el-card>
      <h2>食堂管理</h2>

      <el-table :data="canteenList" style="width: 100%">
        <el-table-column prop="id" label="食堂ID" width="80" />
        <el-table-column prop="name" label="食堂名称" />
        <el-table-column prop="address" label="详细地址" />
        <el-table-column prop="regionName" label="区域" />
        <el-table-column prop="managerName" label="管理者" />
        <el-table-column prop="rating" label="评分" width="80" />
        <el-table-column prop="complaintCount" label="投诉数量" width="100" />
        <el-table-column prop="status" label="状态" width="80">
          <template #default="scope">
            <el-tag :type="scope.row.status === 1 ? 'success' : 'danger'">
              {{ scope.row.status === 1 ? '营业中' : '已下架' }}
            </el-tag>
          </template>
        </el-table-column>
        <el-table-column label="操作" width="150">
          <template #default="scope">
            <el-button 
              type="primary" 
              size="small" 
              @click="handleStatusChange(scope.row)"
            >
              {{ scope.row.status === 1 ? '下架' : '上架' }}
            </el-button>
            <el-button 
              type="info" 
              size="small" 
              @click="viewCanteenDetails(scope.row)"
              style="margin-left: 10px"
            >
              详情
            </el-button>
          </template>
        </el-table-column>
      </el-table>

      <!-- 食堂详情对话框 -->
      <el-dialog
        v-model="detailDialogVisible"
        title="食堂详情"
        width="600px"
      >
        <el-form :model="selectedCanteen" label-width="100px">
          <el-form-item label="食堂名称">
            <el-input v-model="selectedCanteen.name" disabled />
          </el-form-item>
          <el-form-item label="详细地址">
            <el-input v-model="selectedCanteen.address" disabled />
          </el-form-item>
          <el-form-item label="区域">
            <el-input v-model="selectedCanteen.regionName" disabled />
          </el-form-item>
          <el-form-item label="管理者">
            <el-input v-model="selectedCanteen.managerName" disabled />
          </el-form-item>
          <el-form-item label="评分">
            <el-input v-model="selectedCanteen.rating" disabled />
          </el-form-item>
          <el-form-item label="投诉数量">
            <el-input v-model="selectedCanteen.complaintCount" disabled />
          </el-form-item>
          <el-form-item label="状态">
            <el-tag :type="selectedCanteen.status === 1 ? 'success' : 'danger'">
              {{ selectedCanteen.status === 1 ? '营业中' : '已下架' }}
            </el-tag>
          </el-form-item>
        </el-form>
      </el-dialog>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

// 食堂列表
const canteenList = ref([])

// 详情对话框
const detailDialogVisible = ref(false)
const selectedCanteen = ref({})

// 请求头自动携带 token
axios.interceptors.request.use(config => {
  const token = localStorage.getItem('token')
  if (token) {
    config.headers.Authorization = 'Bearer ' + token
  }
  return config
})

// 获取食堂列表
const getCanteenList = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/canteenList')
    if (res.data.code === 0) {
      canteenList.value = res.data.data
    } else {
      ElMessage.error('获取食堂列表失败：' + res.data.message)
    }
  } catch (err) {
    ElMessage.error('获取食堂列表失败：' + (err.response?.data?.message || '服务器异常'))
    console.error(err)
  }
}

// 处理食堂状态变更（上架/下架）
const handleStatusChange = async (canteen) => {
  try {
    const newStatus = canteen.status === 1 ? 0 : 1
    const res = await axios.post('http://192.168.56.100:8080/updateCanteenStatus', {
      canteen_id: canteen.id,
      status: newStatus
    })
    if (res.data.code === 0) {
      ElMessage.success(canteen.status === 1 ? '食堂已下架' : '食堂已上架')
      // 重新获取食堂列表
      getCanteenList()
    } else {
      ElMessage.error('操作失败：' + res.data.message)
    }
  } catch (err) {
    ElMessage.error('操作失败：' + (err.response?.data?.message || '服务器异常'))
    console.error(err)
  }
}

// 查看食堂详情
const viewCanteenDetails = async (canteen) => {
  try {
    // 复用myCanteen接口获取食堂详情
    const res = await axios.get('http://192.168.56.100:8080/myCanteen', {
      params: { user_id: canteen.managerId }
    })
    if (res.data.code === 0) {
      selectedCanteen.value = res.data.data
      selectedCanteen.value.managerName = canteen.managerName
      selectedCanteen.value.status = canteen.status
      detailDialogVisible.value = true
    } else {
      ElMessage.error('获取食堂详情失败：' + res.data.message)
    }
  } catch (err) {
    ElMessage.error('获取食堂详情失败：' + (err.response?.data?.message || '服务器异常'))
    console.error(err)
  }
}

onMounted(() => {
  getCanteenList()
})
</script>

<style scoped>
.canteen-manage {
  padding: 20px;
}
</style>