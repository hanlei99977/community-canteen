<template>
  <div class="canteen-manage">
    <el-card>
      <h2>食堂管理</h2>

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
        <el-table-column label="操作" width="220">
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
            >
              详情
            </el-button>
            <el-button
              type="success"
              size="small"
              @click="openReviewDialog(scope.row)"
            >
              查看评价
            </el-button>
            <el-button
              type="warning"
              size="small"
              @click="openComplaintDialog(scope.row)"
            >
              查看投诉
            </el-button>
          </template>
        </el-table-column>
      </el-table>

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

      <el-dialog
        v-model="reviewDialogVisible"
        :title="'食堂评价 - ' + selectedCanteenName"
        width="700px"
      >
        <el-table :data="reviewList" border max-height="400">
          <el-table-column prop="username" label="评价用户" width="100" />
          <el-table-column prop="rating" label="评分" width="80">
            <template #default="scope">
              <el-rate v-model="scope.row.rating" disabled text-color="#ff9900" />
            </template>
          </el-table-column>
          <el-table-column prop="content" label="评价内容" min-width="200" />
          <el-table-column prop="create_time" label="评价时间" width="170" />
        </el-table>
        <div style="margin-top: 20px; display: flex; justify-content: center;">
          <el-pagination
            v-model:current-page="reviewPage"
            :page-size="pageSize"
            :page-sizes="[15]"
            :total="reviewTotal"
            layout="total, prev, pager, next"
            @current-change="loadReviews"
          />
        </div>
      </el-dialog>

      <el-dialog
        v-model="complaintDialogVisible"
        :title="'食堂投诉 - ' + selectedCanteenName"
        width="800px"
      >
        <el-table :data="complaintList" border max-height="400">
          <el-table-column prop="report_id" label="投诉ID" width="90" />
          <el-table-column prop="username" label="投诉人" width="100" />
          <el-table-column label="类型" width="100">
            <template #default="scope">{{ typeText(scope.row.type) }}</template>
          </el-table-column>
          <el-table-column prop="content" label="投诉内容" min-width="180" />
          <el-table-column label="状态" width="90">
            <template #default="scope">
              <el-tag v-if="scope.row.status === 0" type="warning">未处理</el-tag>
              <el-tag v-else-if="scope.row.status === 1" type="success">已处理</el-tag>
              <el-tag v-else type="info">不予处理</el-tag>
            </template>
          </el-table-column>
          <el-table-column prop="create_time" label="投诉时间" width="170" />
          <el-table-column label="操作" width="120">
            <template #default="scope">
              <el-button
                type="success"
                size="small"
                :disabled="scope.row.status !== 0"
                @click="handleComplaint(scope.row, 1)"
              >
                已处理
              </el-button>
              <el-button
                type="danger"
                size="small"
                :disabled="scope.row.status !== 0"
                @click="handleComplaint(scope.row, 2)"
              >
                不予处理
              </el-button>
            </template>
          </el-table-column>
        </el-table>
        <div style="margin-top: 20px; display: flex; justify-content: center;">
          <el-pagination
            v-model:current-page="complaintPage"
            :page-size="pageSize"
            :page-sizes="[15]"
            :total="complaintTotal"
            layout="total, prev, pager, next"
            @current-change="loadComplaints"
          />
        </div>
      </el-dialog>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted, watch, computed } from 'vue'
import axios from 'axios'
import { ElMessage, ElMessageBox } from 'element-plus'

const canteenList = ref([])
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

const pageSize = 15

const detailDialogVisible = ref(false)
const selectedCanteen = ref({})

const reviewDialogVisible = ref(false)
const reviewList = ref([])
const reviewPage = ref(1)
const reviewTotal = ref(0)

const complaintDialogVisible = ref(false)
const complaintList = ref([])
const complaintPage = ref(1)
const complaintTotal = ref(0)

const selectedCanteenId = ref(null)
const selectedCanteenName = ref('')

const typeText = (type) => {
  const map = {
    1: '食品安全',
    2: '服务态度',
    3: '环境卫生',
    4: '其他'
  }
  return map[type] || '其他'
}

axios.interceptors.request.use(config => {
  const token = localStorage.getItem('token')
  if (token) {
    config.headers.Authorization = 'Bearer ' + token
  }
  return config
})

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

const getCanteenList = async () => {
  const user = JSON.parse(localStorage.getItem('user') || '{}')
  try {
    const res = await axios.get('http://192.168.56.100:8080/canteenList', {
      params: {
        viewer_id: user.user_id,
        city_id: selectedCityId.value,
        district_id: selectedDistrictId.value
      }
    })
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

const handleStatusChange = async (canteen) => {
  const user = JSON.parse(localStorage.getItem('user') || '{}')
  try {
    const newStatus = canteen.status === 1 ? 0 : 1
    const res = await axios.post('http://192.168.56.100:8080/updateCanteenStatus', {
      canteen_id: canteen.id,
      viewer_id: user.user_id,
      status: newStatus
    })
    if (res.data.code === 0) {
      ElMessage.success(canteen.status === 1 ? '食堂已下架' : '食堂已上架')
      getCanteenList()
    } else {
      ElMessage.error('操作失败：' + res.data.message)
    }
  } catch (err) {
    ElMessage.error('操作失败：' + (err.response?.data?.message || '服务器异常'))
    console.error(err)
  }
}

const viewCanteenDetails = async (canteen) => {
  try {
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

const openReviewDialog = (canteen) => {
  selectedCanteenId.value = canteen.id
  selectedCanteenName.value = canteen.name
  reviewPage.value = 1
  reviewDialogVisible.value = true
  loadReviews()
}

const loadReviews = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/canteenReviews', {
      params: {
        canteen_id: selectedCanteenId.value,
        page: reviewPage.value,
        page_size: pageSize
      }
    })
    if (res.data.code === 0) {
      reviewList.value = res.data.data.list || []
      reviewTotal.value = res.data.data.total || 0
    } else {
      ElMessage.error('获取评价列表失败')
    }
  } catch (err) {
    ElMessage.error('获取评价列表失败')
    console.error(err)
  }
}

const openComplaintDialog = (canteen) => {
  selectedCanteenId.value = canteen.id
  selectedCanteenName.value = canteen.name
  complaintPage.value = 1
  complaintDialogVisible.value = true
  loadComplaints()
}

const loadComplaints = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/canteenComplaints', {
      params: {
        canteen_id: selectedCanteenId.value,
        page: complaintPage.value,
        page_size: pageSize
      }
    })
    if (res.data.code === 0) {
      complaintList.value = res.data.data.list || []
      complaintTotal.value = res.data.data.total || 0
    } else {
      ElMessage.error('获取投诉列表失败')
    }
  } catch (err) {
    ElMessage.error('获取投诉列表失败')
    console.error(err)
  }
}

const handleComplaint = async (row, status) => {
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
      loadComplaints()
    } else {
      ElMessage.error(res.data.message || '处理失败')
    }
  } catch (error) {
    if (error !== 'cancel') {
      ElMessage.error('处理失败')
    }
  }
}

watch([selectedCityId, selectedDistrictId], () => {
  getCanteenList()
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
  await getCanteenList()
})
</script>

<style scoped>
.canteen-manage {
  padding: 20px;
}
</style>