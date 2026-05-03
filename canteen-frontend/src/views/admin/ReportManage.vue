<template>
  <div>
    <h2>举报汇总</h2>

    <el-card style="margin-bottom: 20px;">
      <div style="display: flex; gap: 20px; flex-wrap: wrap;">
        <div style="display: flex; align-items: center;">
          <span style="margin-right: 10px;">餐厅范围：</span>
          <el-select v-model="selectedRange" placeholder="全部" clearable style="width: 150px;" @change="handleRangeChange">
            <el-option label="所有餐厅" :value="0" />
            <el-option label="市级所有餐厅" :value="1" />
            <el-option label="区级所有餐厅" :value="2" />
          </el-select>
        </div>
        <div style="display: flex; align-items: center;">
          <span style="margin-right: 10px;">时间维度：</span>
          <el-radio-group v-model="timePeriod" @change="loadReportData">
            <el-radio-button value="day">日</el-radio-button>
            <el-radio-button value="month">月</el-radio-button>
            <el-radio-button value="year">年</el-radio-button>
          </el-radio-group>
        </div>
        <div style="display: flex; align-items: center;">
          <span style="margin-right: 10px;">投诉类型：</span>
          <el-select v-model="complaintType" placeholder="全部" clearable style="width: 150px;" @change="loadReportData">
            <el-option label="全部" :value="0" />
            <el-option label="食品安全" :value="1" />
            <el-option label="服务态度" :value="2" />
            <el-option label="环境卫生" :value="3" />
            <el-option label="其他" :value="4" />
          </el-select>
        </div>
      </div>
    </el-card>

    <el-card style="margin-bottom: 20px;">
      <div style="display: flex; align-items: center; gap: 10px;">
        <span style="font-size: 16px; font-weight: bold;">当前范围未处理投诉总数：</span>
        <el-tag type="danger" size="large" style="font-size: 18px; padding: 8px 20px;">
          {{ unprocessedCount }}
        </el-tag>
      </div>
    </el-card>

    <el-card>
      <div id="reportChart" ref="chartRef" class="chart"></div>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from 'vue'
import axios from 'axios'
import * as echarts from 'echarts'
import { ElMessage } from 'element-plus'

const selectedRange = ref(0)
const timePeriod = ref('day')
const complaintType = ref(0)
const unprocessedCount = ref(0)
const chartRef = ref(null)
const userInfo = ref({
  user_id: 0,
  adminLevel: 0,
  regionId: 0
})
let chart = null

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

const handleRangeChange = () => {
  loadUnprocessedCount()
  loadReportData()
}

const loadUnprocessedCount = async () => {
  const user = getUser()
  try {
    const params = {
      viewer_id: user.user_id,
      range_type: selectedRange.value
    }
    const res = await axios.get('http://192.168.56.100:8080/unprocessedReportCount', { params })
    if (res.data.code === 0) {
      unprocessedCount.value = res.data.data
    }
  } catch (error) {
    console.error('获取未处理投诉数失败:', error)
  }
}

const loadReportData = async () => {
  const user = getUser()
  if (!chartRef.value) return

  if (!chart) {
    chart = echarts.init(chartRef.value)
  }

  try {
    const params = {
      viewer_id: user.user_id,
      range_type: selectedRange.value,
      time_period: timePeriod.value,
      complaint_type: complaintType.value
    }
    const res = await axios.get('http://192.168.56.100:8080/reportSummary', { params })

    if (res.data.code === 0) {
      const data = res.data.data
      renderChart(data.canteen_names, data.complaint_counts)
    } else {
      ElMessage.error(res.data.message || '获取举报汇总数据失败')
    }
  } catch (error) {
    console.error('获取举报汇总数据失败:', error)
    ElMessage.error('网络错误，请稍后重试')
  }
}

const renderChart = (canteenNames, complaintCounts) => {
  if (!chart) {
    chart = echarts.init(chartRef.value)
  }

  const total = complaintCounts.reduce((sum, count) => sum + count, 0)

  const option = {
    title: {
      text: '各食堂投诉分布',
      subtext: `总计: ${total} 条投诉`,
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
      name: '投诉数量',
      type: 'pie',
      radius: ['40%', '70%'],
      avoidLabelOverlap: false,
      itemStyle: {
        borderRadius: 10,
        borderColor: '#fff',
        borderWidth: 2
      },
      label: {
        show: true,
        formatter: '{b}: {c}'
      },
      emphasis: {
        label: {
          show: true,
          fontSize: 16,
          fontWeight: 'bold'
        }
      },
      data: canteenNames.map((name, index) => ({
        name: name,
        value: complaintCounts[index]
      }))
    }]
  }

  chart.setOption(option)
}

const handleResize = () => {
  if (chart) {
    chart.resize()
  }
}

const initUserInfo = () => {
  const user = getUser()
  if (user) {
    userInfo.value = {
      user_id: user.user_id || 0,
      adminLevel: user.adminLevel || 0,
      regionId: user.regionId || 0
    }
  }
}

onMounted(() => {
  initUserInfo()
  loadUnprocessedCount()
  loadReportData()
  window.addEventListener('resize', handleResize)
})

onUnmounted(() => {
  if (chart) {
    chart.dispose()
    chart = null
  }
  window.removeEventListener('resize', handleResize)
})
</script>

<style scoped>
.chart {
  width: 100%;
  height: 500px;
}
</style>