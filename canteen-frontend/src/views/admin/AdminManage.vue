<template>
  <div>

    <!-- 标题 -->
    <h2 style="margin-bottom:20px;">管理员管理</h2>

    <!-- 管理员列表 -->
    <el-table :data="adminList" border style="width: 100%">

      <el-table-column prop="user_id" label="ID" width="80" />

      <el-table-column prop="username" label="姓名" />

      <el-table-column prop="phone" label="电话" />

      <el-table-column prop="level_name" label="等级" />

      <el-table-column prop="region_name" label="管理区域" />

      <!-- 状态 -->
      <el-table-column label="状态">
        <template #default="scope">
          <el-tag :type="scope.row.status === 1 ? 'success' : 'danger'">
            {{ scope.row.status === 1 ? '正常' : '已禁用' }}
          </el-tag>
        </template>
      </el-table-column>

      <!-- 操作 -->
      <el-table-column label="操作" width="150">
        <template #default="scope">
          <el-button
            size="small"
            :type="scope.row.status === 1 ? 'danger' : 'success'"
            @click="handleStatus(scope.row)"
          >
            {{ scope.row.status === 1 ? '禁用' : '解除禁用' }}
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

const adminList = ref([])

// 获取管理员列表
const getAdminList = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/adminList')
    adminList.value = res.data.data ||[]
  } catch (err) {
    ElMessage.error('获取管理员列表失败')
  }
}

// 修改状态
const handleStatus = (row) => {
  const newStatus = row.status === 1 ? 0 : 1

  ElMessageBox.confirm(
    `确定要${newStatus === 0 ? '禁用' : '解除禁用'}该管理员吗？`,
    '提示',
    {
      type: 'warning'
    }
  ).then(async () => {
    try {
      await axios.post('/user/updateStatus', {
        user_id: row.user_id,
        status: newStatus
      })

      row.status = newStatus
      ElMessage.success('操作成功')

    } catch (err) {
      ElMessage.error('操作失败')
    }
  })
}

onMounted(() => {
  getAdminList()
})
</script>

<style scoped>
</style>
