<template>
  <div>

    <!-- 标题 -->
    <div style="display:flex;justify-content: space-between;align-items: center;margin-bottom: 20px;">
      <h2>用餐者管理</h2>
    </div>

    <!-- 区域筛选 -->
    <div style="margin-bottom: 20px; display: flex; gap: 10px;">
      <span>市级区域：</span>
      <el-select v-model="selectedCityId" placeholder="全部" clearable style="width: 150px;" :disabled="cityDisabled" @change="handleCityChange">
        <el-option label="全部" :value="null" />
        <el-option
          v-for="city in cityOptions"
          :key="city.region_id"
          :label="city.region_name"
          :value="city.region_id"
        />
      </el-select>
      <span>区级区域：</span>
      <el-select v-model="selectedDistrictId" placeholder="全部" clearable style="width: 150px;" :disabled="districtDisabled">
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
      <el-table-column prop="region_name" label="区域" />
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
import { ref, onMounted, computed, watch } from 'vue'
import axios from 'axios'
import { ElMessage, ElMessageBox } from 'element-plus'

const activeTab = ref("on")

const allDinerList = ref([])
const cityOptions = ref([])
const districtOptions = ref([])
const selectedCityId = ref(null)
const selectedDistrictId = ref(null)

const userInfo = ref({
  user_id: 0,
  adminLevel: 0,
  regionId: 0,
  regionName: ''
})

const isSystemAdmin = computed(() => userInfo.value.adminLevel === 1)
const isCityAdmin = computed(() => userInfo.value.adminLevel === 2)
const isDistrictAdmin = computed(() => userInfo.value.adminLevel === 3)

const cityDisabled = computed(() => isCityAdmin.value || isDistrictAdmin.value)
const districtDisabled = computed(() => isDistrictAdmin.value || !selectedCityId.value)

const onDinerList = computed(() =>
  allDinerList.value.filter(d => d.status === 1)
)

const offDinerList = computed(() =>
  allDinerList.value.filter(d => d.status === 0)
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

const getDiners = async () => {
  const user = JSON.parse(localStorage.getItem('user') || '{}')
  try {
    const res = await axios.get('http://192.168.56.100:8080/dinerList', {
      params: {
        viewer_id: user.user_id || 0,
        city_id: selectedCityId.value,
        district_id: selectedDistrictId.value
      }
    })
    allDinerList.value = res.data.data || []
  } catch (err) {
    ElMessage.error("获取用餐者失败")
  }
}

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

watch([selectedCityId, selectedDistrictId], () => {
  getDiners()
})

const initRegionByAdminLevel = async () => {
  const storedUser = localStorage.getItem('user')
  if (!storedUser) return

  const user = JSON.parse(storedUser)
  userInfo.value = {
    user_id: user.user_id || 0,
    adminLevel: user.adminLevel || 0,
    regionId: user.regionId || 0,
    regionName: user.regionName || ''
  }

  if (isCityAdmin.value) {
    selectedCityId.value = userInfo.value.regionId
    await loadDistrictOptions(userInfo.value.regionId)
    selectedDistrictId.value = null
  } else if (isDistrictAdmin.value) {
    selectedCityId.value = await findParentCityId(userInfo.value.regionId)
    await loadDistrictOptions(selectedCityId.value)
    selectedDistrictId.value = userInfo.value.regionId
  }
}

const findParentCityId = async (districtId) => {
  for (const city of cityOptions.value) {
    try {
      const res = await axios.get('http://192.168.56.100:8080/getDistrictsByCity', {
        params: { city_id: city.region_id }
      })
      if (res.data.code === 0) {
        const districts = res.data.data || []
        if (districts.some(d => d.region_id === districtId)) {
          return city.region_id
        }
      }
    } catch (e) {}
  }
  return null
}

onMounted(async () => {
  await loadCityOptions()
  await initRegionByAdminLevel()
  await getDiners()
})
</script>

<style scoped>
</style>