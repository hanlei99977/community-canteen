<template>
  <div>

    <!-- 标题 -->
    <div style="display:flex;justify-content: space-between;align-items: center;margin-bottom: 20px;">
      <h2>管理员管理</h2>
    </div>

    <!-- 区域筛选 -->
    <div style="margin-bottom: 20px; display: flex; gap: 10px;">
      <span>市级区域：</span>
      <el-select v-model="selectedCityId" placeholder="全部" clearable style="width: 150px;" @change="handleCityChange">
        <el-option label="全部" :value="null" />
        <el-option
          v-for="city in cityOptions"
          :key="city.region_id"
          :label="city.region_name"
          :value="city.region_id"
        />
      </el-select>
      <span>区级区域：</span>
      <el-select v-model="selectedDistrictId" placeholder="全部" clearable style="width: 150px;" :disabled="!selectedCityId">
        <el-option label="全部" :value="null" />
        <el-option
          v-for="district in districtOptions"
          :key="district.region_id"
          :label="district.region_name"
          :value="district.region_id"
        />
      </el-select>
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
import { ref, onMounted, computed, watch } from 'vue'
import axios from 'axios'
import { ElMessage, ElMessageBox } from 'element-plus'

const activeTab = ref("on")

const allAdminList = ref([])
const cityOptions = ref([])
const districtOptions = ref([])
const selectedCityId = ref(null)
const selectedDistrictId = ref(null)

const onAdminList = computed(() =>
  allAdminList.value.filter(a => a.status === 1)
)

const offAdminList = computed(() =>
  allAdminList.value.filter(a => a.status === 0)
)

const loadCityOptions = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/cityRegionList')
    if (res.data.code === 0) {
      cityOptions.value = res.data.data
    }
  } catch (e) {}
}

const loadDistrictOptions = async (cityId) => {
  if (!cityId) {
    districtOptions.value = []
    return
  }
  try {
    const res = await axios.get('http://192.168.56.100:8080/getDistrictsByCity', {
      params: { city_id: cityId }
    })
    if (res.data.code === 0) {
      districtOptions.value = res.data.data
    }
  } catch (e) {}
}

const handleCityChange = (cityId) => {
  selectedDistrictId.value = null
  loadDistrictOptions(cityId)
}

const getAdmins = async () => {
  const user = JSON.parse(localStorage.getItem('user') || '{}')
  try {
    const res = await axios.get('http://192.168.56.100:8080/adminList', {
      params: {
        viewer_id: user.user_id || 0,
        city_id: selectedCityId.value,
        district_id: selectedDistrictId.value
      }
    })
    allAdminList.value = res.data.data || []
  } catch (err) {
    ElMessage.error("获取管理员失败")
  }
}

const disableAdmin = (user_id, level_name) => {
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

watch([selectedCityId, selectedDistrictId], () => {
  getAdmins()
})

onMounted(async () => {
  await loadCityOptions()
  await getAdmins()
})
</script>

<style scoped>
</style>