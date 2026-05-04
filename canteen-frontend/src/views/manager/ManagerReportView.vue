<template>
  <div>
    <h2>举报汇总</h2>

    <el-card style="margin-bottom: 20px;">
      <div style="display: flex; gap: 20px; flex-wrap: wrap; align-items: center;">
        <div style="display: flex; align-items: center;">
          <span style="margin-right: 10px;">市级区域：</span>
          <el-select v-model="selectedCityId" disabled placeholder="请选择市级区域" style="width: 150px;">
            <el-option v-for="city in cityList" :key="city.region_id" :label="city.region_name" :value="city.region_id" />
          </el-select>
        </div>
        <div style="display: flex; align-items: center;">
          <span style="margin-right: 10px;">区级区域：</span>
          <el-select v-model="selectedDistrictId" disabled placeholder="请选择区级区域" style="width: 150px;">
            <el-option v-for="district in districtList" :key="district.region_id" :label="district.region_name" :value="district.region_id" />
          </el-select>
        </div>
        <div style="display: flex; align-items: center;">
          <span style="margin-right: 10px;">餐厅：</span>
          <el-select v-model="selectedCanteenId" disabled placeholder="请选择餐厅" style="width: 180px;">
            <el-option v-for="canteen in canteenList" :key="canteen.id" :label="canteen.name" :value="canteen.id" />
          </el-select>
        </div>
        <div style="display: flex; align-items: center;">
          <span style="margin-right: 10px;">时间范围：</span>
          <el-radio-group v-model="timeRange" @change="loadData">
            <el-radio-button :value="7">近7天</el-radio-button>
            <el-radio-button :value="30">近30天</el-radio-button>
            <el-radio-button :value="365">近12月</el-radio-button>
          </el-radio-group>
        </div>
      </div>
    </el-card>

    <el-card style="margin-bottom: 20px;">
      <div style="display: flex; gap: 40px; justify-content: center;">
        <div style="text-align: center;">
          <div style="font-size: 14px; color: #909399; margin-bottom: 5px;">未处理投诉数</div>
          <div style="font-size: 32px; font-weight: bold; color: #F56C6C;">{{ statistics.unprocessed_count }}</div>
        </div>
        <div style="text-align: center;">
          <div style="font-size: 14px; color: #909399; margin-bottom: 5px;">总投诉数</div>
          <div style="font-size: 32px; font-weight: bold; color: #409EFF;">{{ statistics.total_count }}</div>
        </div>
        <div style="text-align: center;">
          <div style="font-size: 14px; color: #909399; margin-bottom: 5px;">今日新增</div>
          <div style="font-size: 32px; font-weight: bold; color: #67C23A;">{{ statistics.today_count }}</div>
        </div>
      </div>
    </el-card>

    <el-card style="margin-bottom: 20px;">
      <div id="trendChart" ref="trendChartRef" class="chart"></div>
    </el-card>

    <el-card style="margin-bottom: 20px;">
      <div id="typeChart" ref="typeChartRef" class="chart"></div>
    </el-card>

    <el-dialog v-model="complaintDialogVisible" title="投诉列表" width="900px">
      <div style="margin-bottom: 15px; font-size: 14px; color: #606266;">
        {{ dialogTitle }}
      </div>
      <el-table :data="complaintList" border max-height="400">
        <el-table-column prop="report_id" label="投诉ID" width="80" />
        <el-table-column prop="username" label="投诉人" width="100" />
        <el-table-column label="类型" width="100">
          <template #default="scope">{{ typeText(scope.row.type) }}</template>
        </el-table-column>
        <el-table-column prop="canteen_name" label="食堂" width="120" />
        <el-table-column prop="content" label="投诉内容" min-width="150" />
        <el-table-column label="状态" width="80">
          <template #default="scope">
            <el-tag :type="scope.row.status === 0 ? 'danger' : 'success'" size="small">
              {{ scope.row.status === 0 ? '未处理' : '已处理' }}
            </el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="create_time" label="时间" width="170" />
      </el-table>
      <div style="margin-top: 20px; display: flex; justify-content: center;">
        <el-pagination
          v-model:current-page="complaintPage"
          :page-size="15"
          :total="complaintTotal"
          layout="total, prev, pager, next"
          @current-change="loadComplaintDialog"
        />
      </div>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from 'vue'
import axios from 'axios'
import * as echarts from 'echarts'
import { ElMessage } from 'element-plus'

const selectedCityId = ref(0)
const selectedDistrictId = ref(0)
const selectedCanteenId = ref(0)
const cityList = ref([])
const districtList = ref([])
const canteenList = ref([])
const timeRange = ref(7)

const statistics = ref({
  unprocessed_count: 0,
  total_count: 0,
  today_count: 0
})

const trendChartRef = ref(null)
const typeChartRef = ref(null)

let trendChart = null
let typeChart = null

const complaintDialogVisible = ref(false)
const complaintList = ref([])
const complaintPage = ref(1)
const complaintTotal = ref(0)
const dialogTitle = ref('')
const dialogCanteenId = ref(null)
const dialogType = ref(null)

const typeMap = {
  1: '食品安全',
  2: '服务态度',
  3: '环境卫生',
  4: '其他'
}

axios.interceptors.request.use(config => {
  const token = localStorage.getItem('token')
  if (token) {
    config.headers.Authorization = 'Bearer ' + token
  }
  return config
})

const getUser = () => JSON.parse(localStorage.getItem('user'))

const loadCityList = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/cityRegionList')
    if (res.data.code === 0) {
      cityList.value = res.data.data || []
    }
  } catch (error) {
    console.error('获取城市列表失败:', error)
  }
}

const loadDistrictList = async () => {
  try {
    const params = { city_id: selectedCityId.value || 0 }
    const res = await axios.get('http://192.168.56.100:8080/getDistrictsByCity', { params })
    if (res.data.code === 0) {
      districtList.value = res.data.data || []
    }
  } catch (error) {
    console.error('获取区县列表失败:', error)
  }
}

const loadAllDistricts = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/districtRegionList')
    if (res.data.code === 0) {
      districtList.value = res.data.data || []
    }
  } catch (error) {
    console.error('获取所有区县列表失败:', error)
  }
}

const loadCanteenList = async () => {
  try {
    const params = {
      city_id: selectedCityId.value || 0,
      district_id: selectedDistrictId.value || 0
    }
    const res = await axios.get('http://192.168.56.100:8080/canteenList', { params })
    if (res.data.code === 0) {
      canteenList.value = res.data.data || []
    }
  } catch (error) {
    console.error('获取食堂列表失败:', error)
  }
}

const loadStatistics = async () => {
  const user = getUser()
  try {
    const params = {
      viewer_id: user.user_id,
      city_id: selectedCityId.value,
      district_id: selectedDistrictId.value,
      canteen_id: selectedCanteenId.value,
      days: timeRange.value
    }
    const res = await axios.get('http://192.168.56.100:8080/reportStatistics', { params })
    if (res.data.code === 0) {
      statistics.value = res.data.data
    }
  } catch (error) {
    console.error('获取统计数据失败:', error)
  }
}

const loadTrendData = async () => {
  const user = getUser()
  if (!trendChartRef.value) return

  if (!trendChart) {
    trendChart = echarts.init(trendChartRef.value)
  }

  try {
    const params = {
      viewer_id: user.user_id,
      city_id: selectedCityId.value,
      district_id: selectedDistrictId.value,
      canteen_id: selectedCanteenId.value,
      days: timeRange.value
    }
    const res = await axios.get('http://192.168.56.100:8080/reportTrend', { params })
    if (res.data.code === 0) {
      const data = res.data.data
      renderTrendChart(data.dates, data.counts)
    }
  } catch (error) {
    console.error('获取趋势数据失败:', error)
  }
}

const loadTypeData = async () => {
  const user = getUser()
  if (!typeChartRef.value) return

  if (!typeChart) {
    typeChart = echarts.init(typeChartRef.value)
  }

  try {
    const params = {
      viewer_id: user.user_id,
      city_id: selectedCityId.value,
      district_id: selectedDistrictId.value,
      canteen_id: selectedCanteenId.value,
      days: timeRange.value
    }
    const res = await axios.get('http://192.168.56.100:8080/reportTypeDistribution', { params })
    if (res.data.code === 0) {
      const data = res.data.data
      renderTypeChart(data.type_names, data.type_counts, data.total)
    }
  } catch (error) {
    console.error('获取类型分布失败:', error)
  }
}

const renderTrendChart = (dates, counts) => {
  if (!trendChart) return

  const option = {
    title: {
      text: '投诉趋势',
      left: 'center'
    },
    tooltip: {
      trigger: 'axis'
    },
    xAxis: {
      type: 'category',
      data: dates,
      axisLabel: {
        rotate: 45
      }
    },
    yAxis: {
      type: 'value',
      name: '投诉数'
    },
    series: [{
      name: '投诉数',
      type: 'line',
      smooth: true,
      data: counts,
      areaStyle: {
        color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
          { offset: 0, color: 'rgba(64, 158, 255, 0.5)' },
          { offset: 1, color: 'rgba(64, 158, 255, 0.1)' }
        ])
      },
      lineStyle: {
        color: '#409EFF'
      },
      itemStyle: {
        color: '#409EFF'
      }
    }],
    grid: {
      left: '3%',
      right: '4%',
      bottom: '12%',
      containLabel: true
    }
  }

  trendChart.setOption(option)
}

const renderTypeChart = (typeNames, typeCounts, total) => {
  if (!typeChart) return

  const colorList = ['#F56C6C', '#E6A23C', '#67C23A', '#909399']

  const option = {
    title: {
      text: '投诉类型分布',
      subtext: `总数: ${total}`,
      left: 'center'
    },
    tooltip: {
      trigger: 'item',
      formatter: '{b}: {c} ({d}%)'
    },
    legend: {
      orient: 'vertical',
      left: 'left',
      top: 'middle'
    },
    series: [{
      name: '投诉类型',
      type: 'pie',
      radius: ['45%', '70%'],
      center: ['50%', '50%'],
      avoidLabelOverlap: true,
      itemStyle: {
        borderRadius: 8,
        borderColor: '#fff',
        borderWidth: 2
      },
      label: {
        show: true,
        formatter: '{b}: {c}',
        position: 'outside'
      },
      emphasis: {
        label: {
          show: true,
          fontSize: 14,
          fontWeight: 'bold'
        }
      },
      data: typeNames.map((name, index) => ({
        name: name,
        value: typeCounts[index],
        itemStyle: { color: colorList[index % colorList.length] }
      }))
    }],
    graphic: {
      elements: [{
        type: 'text',
        left: 'center',
        top: '40%',
        style: {
          text: total.toString(),
          textAlign: 'center',
          fill: '#333',
          fontSize: 28,
          fontWeight: 'bold'
        }
      }, {
        type: 'text',
        left: 'center',
        top: '55%',
        style: {
          text: '总投诉',
          textAlign: 'center',
          fill: '#999',
          fontSize: 14
        }
      }]
    }
  }

  typeChart.setOption(option)

  typeChart.off('click')
  typeChart.on('click', (params) => {
    if (params.name && params.name !== '总投诉') {
      const typeIndex = ['食品安全', '服务态度', '环境卫生', '其他'].indexOf(params.name)
      if (typeIndex !== -1) {
        dialogTitle.value = `类型: ${params.name}`
        dialogType.value = typeIndex + 1
        dialogCanteenId.value = selectedCanteenId.value || null
        complaintPage.value = 1
        complaintDialogVisible.value = true
        loadComplaintDialog()
      }
    }
  })
}

const loadComplaintDialog = async () => {
  const user = getUser()
  try {
    let canteenId = 0
    if (dialogCanteenId.value !== null && dialogCanteenId.value !== undefined) {
      if (typeof dialogCanteenId.value === 'number') {
        canteenId = dialogCanteenId.value
      } else {
        const canteen = canteenList.value.find(c => c.name === dialogCanteenId.value)
        if (canteen) {
          canteenId = canteen.id
        }
      }
    }

    const params = {
      viewer_id: user.user_id,
      city_id: selectedCityId.value,
      district_id: selectedDistrictId.value,
      canteen_id: canteenId,
      type: dialogType.value || 0,
      days: timeRange.value,
      page: complaintPage.value,
      page_size: 15
    }
    const res = await axios.get('http://192.168.56.100:8080/filteredReports', { params })
    if (res.data.code === 0) {
      complaintList.value = res.data.data.list || []
      complaintTotal.value = res.data.data.total || 0
    }
  } catch (error) {
    console.error('获取投诉列表失败:', error)
  }
}

const typeText = (type) => {
  return typeMap[type] || '未知'
}

const loadData = () => {
  loadStatistics()
  loadTrendData()
  loadTypeData()
}

const handleResize = () => {
  if (trendChart) trendChart.resize()
  if (typeChart) typeChart.resize()
}

onMounted(async () => {
  const user = getUser()
  console.log('ManagerReport user:', user)
  if (user) {
    const canteenId = user.canteen_id || 0
    if (canteenId > 0) {
      try {
        const res = await axios.get('http://192.168.56.100:8080/myCanteen', { params: { user_id: user.user_id } })
        console.log('myCanteen response:', res.data)
        if (res.data.code === 0) {
          const canteenData = res.data.data
          selectedCanteenId.value = canteenData.id
          const regionId = canteenData.regionId
          console.log('regionId from canteen:', regionId)
          await loadCityList()
          await loadAllDistricts()
          console.log('districtList:', districtList.value)
          const district = districtList.value.find(d => d.region_id === regionId)
          console.log('found district:', district)
          if (district) {
            selectedDistrictId.value = regionId
            selectedCityId.value = district.parent_id
          }
          await loadCanteenList()
          loadData()
        }
      } catch (error) {
        console.error('获取食堂信息失败:', error)
      }
    }
  }
  window.addEventListener('resize', handleResize)
})

onUnmounted(() => {
  if (trendChart) {
    trendChart.dispose()
    trendChart = null
  }
  if (typeChart) {
    typeChart.dispose()
    typeChart = null
  }
  window.removeEventListener('resize', handleResize)
})
</script>

<style scoped>
.chart {
  width: 100%;
  height: 350px;
}
</style>
