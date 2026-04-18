<template>
  <div>
    <h2>用餐偏好</h2>
    
    <!-- 时间维度选择 -->
    <div class="filter-container">
      <div class="filter-buttons">
        <span class="filter-label">时间维度</span>
        <el-radio-group v-model="timeDimension" @change="loadDiningPreference">
          <el-radio-button label="year">年</el-radio-button>
          <el-radio-button label="quarter">季</el-radio-button>
          <el-radio-button label="month">月</el-radio-button>
          <el-radio-button label="week">周</el-radio-button>
          <el-radio-button label="day">日</el-radio-button>
        </el-radio-group>
      </div>
    </div>
    
    <!-- 顶部数据卡片 -->
    <div class="stats-cards">
      <el-card class="stats-card">
        <div class="card-content">
          <div class="card-title">消费总额</div>
          <div class="card-value">{{ totalAmount.toFixed(2) }}元</div>
        </div>
      </el-card>
      <el-card class="stats-card">
        <div class="card-content">
          <div class="card-title">订单数</div>
          <div class="card-value">{{ orderCount }}</div>
        </div>
      </el-card>
      <el-card class="stats-card">
        <div class="card-content">
          <div class="card-title">消费餐厅数</div>
          <div class="card-value">{{ canteenCount }}家</div>
        </div>
      </el-card>
    </div>
    
    <!-- 扇形图 -->
    <div class="chart-container">
      <el-card class="chart-card">
        <template #header>
          <div class="card-header">
            <span>餐厅消费偏好</span>
          </div>
        </template>
        <div id="canteenChart" ref="canteenChartRef" class="chart"></div>
      </el-card>
      <el-card class="chart-card">
        <template #header>
          <div class="card-header">
            <span>菜品消费偏好</span>
          </div>
        </template>
        <div id="dishChart" ref="dishChartRef" class="chart"></div>
      </el-card>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from 'vue'
import axios from 'axios'
import * as echarts from 'echarts'
import { ElMessage } from 'element-plus'

// 数据
const timeDimension = ref('month') // 默认显示最近一月
const totalAmount = ref(0)
const orderCount = ref(0)
const canteenCount = ref(0)
const canteenChartRef = ref(null)
const dishChartRef = ref(null)
let canteenChart = null
let dishChart = null

// 获取用户信息
const getUser = () => JSON.parse(localStorage.getItem('user'))

// 加载用餐偏好数据
const loadDiningPreference = async () => {
  const user = getUser()
  if (!user || !user.user_id) {
    ElMessage.error('用户未登录')
    return
  }
  
  try {
    const res = await axios.get(
      'http://192.168.56.100:8080/diningPreference',
      {
        params: {
          user_id: user.user_id,
          time_dimension: timeDimension.value
        }
      }
    )
    
    if (res.data.code === 0) {
      // 更新顶部数据
      totalAmount.value = res.data.data.total_amount
      orderCount.value = res.data.data.order_count
      canteenCount.value = res.data.data.canteen_count
      
      // 渲染餐厅消费偏好图表
      renderCanteenChart(res.data.data.canteen_consumption)
      
      // 渲染菜品消费偏好图表
      renderDishChart(res.data.data.dish_consumption)
    } else {
      ElMessage.error(res.data.msg || '获取用餐偏好数据失败')
    }
  } catch (error) {
    console.error('获取用餐偏好数据失败:', error)
    ElMessage.error('网络错误，请稍后重试')
  }
}

// 渲染餐厅消费偏好图表
const renderCanteenChart = (canteenConsumption) => {
  if (!canteenChartRef.value) return
  
  if (!canteenChart) {
    canteenChart = echarts.init(canteenChartRef.value)
  }
  
  const data = canteenConsumption.map(item => ({
    name: item.name,
    value: item.count
  }))
  
  const option = {
    tooltip: {
      trigger: 'item',
      formatter: '{a} <br/>{b}: {c} ({d}%)'
    },
    legend: {
      orient: 'vertical',
      left: 'left',
      data: canteenConsumption.map(item => item.name)
    },
    series: [
      {
        name: '餐厅消费次数',
        type: 'pie',
        radius: '60%',
        center: ['50%', '50%'],
        data: data,
        emphasis: {
          itemStyle: {
            shadowBlur: 10,
            shadowOffsetX: 0,
            shadowColor: 'rgba(0, 0, 0, 0.5)'
          }
        }
      }
    ]
  }
  
  canteenChart.setOption(option)
}

// 渲染菜品消费偏好图表
const renderDishChart = (dishConsumption) => {
  if (!dishChartRef.value) return
  
  if (!dishChart) {
    dishChart = echarts.init(dishChartRef.value)
  }
  
  const data = dishConsumption.map(item => ({
    name: item.name,
    value: item.count
  }))
  
  const option = {
    tooltip: {
      trigger: 'item',
      formatter: '{a} <br/>{b}: {c} ({d}%)'
    },
    legend: {
      orient: 'vertical',
      left: 'left',
      data: dishConsumption.map(item => item.name),
      type: 'scroll'
    },
    series: [
      {
        name: '菜品消费次数',
        type: 'pie',
        radius: '60%',
        center: ['50%', '50%'],
        data: data,
        emphasis: {
          itemStyle: {
            shadowBlur: 10,
            shadowOffsetX: 0,
            shadowColor: 'rgba(0, 0, 0, 0.5)'
          }
        }
      }
    ]
  }
  
  dishChart.setOption(option)
}

// 监听窗口大小变化，重新调整图表大小
const handleResize = () => {
  if (canteenChart) {
    canteenChart.resize()
  }
  if (dishChart) {
    dishChart.resize()
  }
}

// 初始化
onMounted(() => {
  loadDiningPreference()
  window.addEventListener('resize', handleResize)
})

// 组件卸载时清理
onUnmounted(() => {
  if (canteenChart) {
    canteenChart.dispose()
  }
  if (dishChart) {
    dishChart.dispose()
  }
  window.removeEventListener('resize', handleResize)
})
</script>

<style scoped>
.filter-container {
  margin-bottom: 20px;
}

.filter-buttons {
  display: flex;
  align-items: center;
  gap: 10px;
}

.filter-label {
  font-size: 14px;
  color: #606266;
  margin-right: 8px;
}

.el-radio-group {
  margin-right: 10px;
}

.stats-cards {
  display: flex;
  gap: 20px;
  margin-bottom: 20px;
}

.stats-card {
  flex: 1;
  min-width: 200px;
}

.card-content {
  text-align: center;
}

.card-title {
  font-size: 16px;
  margin-bottom: 10px;
  color: #606266;
}

.card-value {
  font-size: 24px;
  font-weight: bold;
  color: #409EFF;
}

.chart-container {
  display: flex;
  gap: 20px;
  margin-top: 20px;
}

.chart-card {
  flex: 1;
  min-width: 400px;
}

.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.chart {
  width: 100%;
  height: 400px;
}

@media screen and (max-width: 768px) {
  .chart-container {
    flex-direction: column;
  }
  
  .chart-card {
    min-width: 100%;
  }
}
</style>