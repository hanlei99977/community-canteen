<template>
  <div>
    <h2>食堂管理者申请审核</h2>

    <el-table :data="applyList" style="width: 100%">
      <el-table-column prop="apply_id" label="申请ID" width="90" />
      <el-table-column prop="username" label="用户名" />
      <el-table-column prop="age" label="年龄" width="80" />
      <el-table-column prop="phone" label="手机号" />
      <el-table-column prop="canteen_name" label="食堂名称" />
      <el-table-column label="申请状态" width="120">
        <template #default="scope">
          <el-tag v-if="scope.row.status === 0" type="warning">未处理</el-tag>
          <el-tag v-else-if="scope.row.status === 1" type="success">已通过</el-tag>
          <el-tag v-else type="danger">已拒绝</el-tag>
        </template>
      </el-table-column>
      <el-table-column prop="apply_time" label="申请时间" width="180" />
      <el-table-column prop="review_time" label="审核时间" width="180" />
      <el-table-column prop="reviewer_name" label="审核人" width="120" />

      <el-table-column label="操作" width="200">
        <template #default="scope">
          <el-button
            type="success"
            size="small"
            :disabled="scope.row.status !== 0"
            @click="reviewApply(scope.row, 1)"
          >
            通过
          </el-button>
          <el-button
            type="danger"
            size="small"
            :disabled="scope.row.status !== 0"
            @click="reviewApply(scope.row, 2)"
          >
            拒绝
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

const applyList = ref([])

const loadApplyList = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/managerApplyList')
    if (res.data.code === 0) {
      applyList.value = res.data.data
    } else {
      ElMessage.error('加载食堂管理者申请失败')
    }
  } catch (e) {
    ElMessage.error('请求失败')
  }
}

const reviewApply = async (row, status) => {
  const actionText = status === 1 ? '通过' : '拒绝'
  const user = JSON.parse(localStorage.getItem('user') || '{}')

  try {
    await ElMessageBox.confirm(
      `确认${actionText}申请人 ${row.username} 的食堂管理者申请吗？`,
      '审核确认',
      { type: 'warning' }
    )

    const res = await axios.post('http://192.168.56.100:8080/managerApplyReview', {
      apply_id: row.apply_id,
      reviewer_id: user.user_id,
      status
    })

    if (res.data.code === 0) {
      ElMessage.success(`已${actionText}`)
      loadApplyList()
    } else {
      ElMessage.error(res.data.message || '审核失败')
    }
  } catch (e) {}
}

onMounted(() => {
  loadApplyList()
})
</script>
