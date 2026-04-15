<template>
  <div>
    <h2>投诉处理</h2>

    <el-table :data="reportList" border>
      <el-table-column prop="report_id" label="投诉ID" width="90" />
      <el-table-column prop="username" label="投诉人" width="100" />
      <el-table-column prop="canteen_name" label="食堂" width="140" />
      <el-table-column label="类型" width="120">
        <template #default="scope">{{ typeText(scope.row.type) }}</template>
      </el-table-column>
      <el-table-column prop="content" label="投诉内容" min-width="220" />
      <el-table-column label="状态" width="100">
        <template #default="scope">
          <el-tag v-if="scope.row.status === 0" type="warning">未处理</el-tag>
          <el-tag v-else-if="scope.row.status === 1" type="success">已处理</el-tag>
          <el-tag v-else type="info">不予处理</el-tag>
        </template>
      </el-table-column>
      <el-table-column prop="create_time" label="投诉时间" width="170" />
      <el-table-column prop="handler_name" label="处理人" width="100" />
      <el-table-column prop="handle_time" label="处理时间" width="170" />
      <el-table-column label="操作" width="180">
        <template #default="scope">
          <el-button
            type="success"
            size="small"
            :disabled="scope.row.status !== 0"
            @click="handle(scope.row, 1)"
          >
            已处理
          </el-button>
          <el-button
            type="danger"
            size="small"
            :disabled="scope.row.status !== 0"
            @click="handle(scope.row, 2)"
          >
            不予处理
          </el-button>
        </template>
      </el-table-column>
    </el-table>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import axios from 'axios'
import { ElMessage, ElMessageBox } from 'element-plus'

const reportList = ref([])

const typeText = (type) => {
  const map = {
    1: '食品安全',
    2: '服务态度',
    3: '环境卫生',
    4: '其他'
  }
  return map[type] || '其他'
}

const loadReports = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/reportList')
    if (res.data.code === 0) {
      reportList.value = res.data.data || []
    }
  } catch (error) {
    ElMessage.error('获取投诉列表失败')
  }
}

const handle = async (row, status) => {
  const text = status === 1 ? '已处理' : '不予处理'
  try {
    await ElMessageBox.confirm(`确认将投诉 #${row.report_id} 标记为${text}吗？`, '提示', { type: 'warning' })

    const user = JSON.parse(localStorage.getItem('user') || '{}')
    const res = await axios.post('http://192.168.56.100:8080/reportHandle', {
      report_id: row.report_id,
      status,
      handler_id: user.user_id
    })

    if (res.data.code === 0) {
      ElMessage.success('处理成功')
      loadReports()
    } else {
      ElMessage.error(res.data.message || '处理失败')
    }
  } catch (error) {
    if (error !== 'cancel') {
      ElMessage.error('处理失败')
    }
  }
}

onMounted(() => {
  loadReports()
})
</script>
