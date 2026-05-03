<template>
  <div>

    <!-- 标题 -->
    <div style="display:flex;justify-content: space-between;align-items: center;margin-bottom: 20px;">
      <h2>用餐者管理</h2>
    </div>

    <!-- Tab -->
    <el-tabs v-model="activeTab">
      <el-tab-pane label="正常用餐者" name="on" />
      <el-tab-pane label="已禁用用餐者" name="off" />
    </el-tabs>

    <!-- 表格 -->
    <el-table
      :data="activeTab === 'on' ? onDinerList : offDinerList"
      border
    >

      <el-table-column prop="user_id" label="ID" width="80" />
      <el-table-column prop="username" label="姓名" />
      <el-table-column prop="age" label="年龄" width="80" />
      <el-table-column prop="phone" label="电话" />
      <el-table-column prop="address" label="地址" />

      <el-table-column prop="disease_history" label="疾病史" />
      <el-table-column prop="taste_preference" label="口味偏好" />

      <!-- 操作 -->
      <el-table-column label="操作" width="150">
        <template #default="scope">

          <!-- 正常 → 禁用 -->
          <el-button
            v-if="activeTab === 'on'"
            type="danger"
            size="small"
            @click="disableDiner(scope.row.user_id)"
          >
            禁用
          </el-button>

          <!-- 禁用 → 启用 -->
          <el-button
            v-else
            type="success"
            size="small"
            @click="enableDiner(scope.row.user_id)"
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

// 所有用餐者
const allDinerList = ref([])

// ================= 分组 =================
const onDinerList = computed(() =>
  allDinerList.value.filter(d => d.status === 1)
)

const offDinerList = computed(() =>
  allDinerList.value.filter(d => d.status === 0)
)

// ================= 获取用餐者 =================
const getDiners = async () => {
  const user = JSON.parse(localStorage.getItem('user') || '{}')
  try {
    const res = await axios.get('http://192.168.56.100:8080/dinerList', {
      params: {
        viewer_id: user.user_id || 0
      }
    })
    allDinerList.value = res.data.data || []
  } catch (err) {
    ElMessage.error("获取用餐者失败")
  }
}

// ================= 禁用 =================
const disableDiner = (user_id) => {
  ElMessageBox.confirm("确定要禁用该用餐者吗？", "提示", {
    type: "warning"
  }).then(async () => {

    await axios.post('http://192.168.56.100:8080/updateStatus', {
      user_id,
      status: 0
    })

    ElMessage.success("已禁用")
    getDiners()
  })
}

// ================= 启用 =================
const enableDiner = (user_id) => {
  ElMessageBox.confirm("确定要启用该用餐者吗？", "提示", {
    type: "success"
  }).then(async () => {

    await axios.post('http://192.168.56.100:8080/updateStatus', {
      user_id,
      status: 1
    })

    ElMessage.success("已启用")
    getDiners()
  })
}

onMounted(() => {
  getDiners()
})
</script>

<style scoped>
</style>
