<template>
  <div>
    <h2>管理员申请审核</h2>

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

    <el-table :data="applyList" style="width: 100%">
      <el-table-column prop="apply_id" label="申请ID" width="90" />
      <el-table-column prop="username" label="用户名" />
      <el-table-column prop="age" label="年龄" width="80" />
      <el-table-column prop="phone" label="手机号" />
      <el-table-column prop="level_name" label="申请级别" />
      <el-table-column prop="region_name" label="申请区域" />
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
import { ref, onMounted, watch, computed } from 'vue'
import axios from 'axios'
import { ElMessage, ElMessageBox } from 'element-plus'

const applyList = ref([])
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

const loadApplyList = async () => {
  const user = JSON.parse(localStorage.getItem('user') || '{}')
  try {
    const res = await axios.get('http://192.168.56.100:8080/adminApplyList', {
      params: {
        reviewer_id: user.user_id || 0,
        city_id: selectedCityId.value,
        district_id: selectedDistrictId.value
      }
    })
    if (res.data.code === 0) {
      applyList.value = res.data.data
    } else {
      ElMessage.error('加载管理员申请失败')
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
      `确认${actionText}申请人 ${row.username} 的管理员申请吗？`,
      '审核确认',
      { type: 'warning' }
    )

    const res = await axios.post('http://192.168.56.100:8080/adminApplyReview', {
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

watch([selectedCityId, selectedDistrictId], () => {
  loadApplyList()
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
    selectedDistrictId.value = null
  } else if (isDistrictAdmin.value) {
    selectedCityId.value = await findParentCityId(userInfo.value.regionId)
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
  await loadApplyList()
})
</script>