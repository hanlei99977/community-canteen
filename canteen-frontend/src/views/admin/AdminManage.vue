<template>
  <div>

    <!-- 标题 -->
    <div style="display:flex;justify-content: space-between;align-items: center;margin-bottom: 20px;">
      <h2>管理员管理</h2>
    </div>

    <!-- Tab -->
    <el-tabs v-model="activeTab">
      <el-tab-pane label="正常管理员" name="on" />
      <el-tab-pane label="已禁用管理员" name="off" />
    </el-tabs>

    <!-- 表格 -->
    <el-table
      :data="activeTab === 'on' ? onAdminList : offAdminList"
      border
    >

      <el-table-column prop="user_id" label="ID" width="80" />
      <el-table-column prop="username" label="姓名" />
      <el-table-column prop="phone" label="电话" />
      <el-table-column prop="level_name" label="等级" />
      <el-table-column prop="region_name" label="管理区域" />

      <!-- 操作 -->
      <el-table-column label="操作" width="150">
        <template #default="scope">

          <!-- 正常 → 禁用 -->
          <el-button
            v-if="activeTab === 'on'"
            type="danger"
            size="small"
            @click="disableAdmin(scope.row.user_id, scope.row.level_name)"
          >
            禁用
          </el-button>

          <!-- 禁用 → 启用 -->
          <el-button
            v-else
            type="success"
            size="small"
            @click="enableAdmin(scope.row.user_id)"
          >
            启用
          </el-button>

        </template>
      </el-table-column>

    </el-table>

  </div>
</template>

<script setup>
import { ref, onMounted, computed } from 'vue'
import axios from 'axios'
import { ElMessage, ElMessageBox } from 'element-plus'

// 当前Tab
const activeTab = ref("on")

// 所有管理员
const allAdminList = ref([])

// ================= 分组 =================
const onAdminList = computed(() =>
  allAdminList.value.filter(a => a.status === 1)
)

const offAdminList = computed(() =>
  allAdminList.value.filter(a => a.status === 0)
)

// ================= 获取管理员 =================
const getAdmins = async () => {
  const user = JSON.parse(localStorage.getItem('user') || '{}')
  try {
    const res = await axios.get('http://192.168.56.100:8080/adminList', {
      params: {
        viewer_id: user.user_id || 0
      }
    })
    allAdminList.value = res.data.data || []
  } catch (err) {
    ElMessage.error("获取管理员失败")
  }
}

// ================= 禁用 =================
const disableAdmin = (user_id, level_name) => {

  // 系统管理员保护
  if (level_name === '系统管理员') {
    ElMessage.warning("系统管理员不能被禁用")
    return
  }

  ElMessageBox.confirm("确定要禁用该管理员吗？", "提示", {
    type: "warning"
  }).then(async () => {

    await axios.post('http://192.168.56.100:8080/updateStatus', {
      user_id,
      status: 0
    })

    ElMessage.success("已禁用")
    getAdmins()
  })
}

// ================= 启用 =================
const enableAdmin = (user_id) => {
  ElMessageBox.confirm("确定要启用该管理员吗？", "提示", {
    type: "success"
  }).then(async () => {

    await axios.post('http://192.168.56.100:8080/updateStatus', {
      user_id,
      status: 1
    })

    ElMessage.success("已启用")
    getAdmins()
  })
}

onMounted(() => {
  getAdmins()
})
</script>

<style scoped>
</style>
