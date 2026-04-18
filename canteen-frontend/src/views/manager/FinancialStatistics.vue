<template>
  <div>
    <h2>财务统计</h2>
    
    <!-- 顶部数据卡片 -->
    <div class="stats-cards">
      <el-card class="stats-card income-card">
        <div class="card-content">
          <div class="card-title">今日收入</div>
          <div class="card-value">{{ todayIncome.toFixed(2) }}元</div>
        </div>
      </el-card>
      <el-card class="stats-card expense-card">
        <div class="card-content">
          <div class="card-title">今日支出</div>
          <div class="card-value">{{ todayExpense.toFixed(2) }}元</div>
        </div>
      </el-card>
      <el-card class="stats-card profit-card">
        <div class="card-content">
          <div class="card-title">今日利润</div>
          <div class="card-value" :class="{ 'profit-positive': todayProfit >= 0, 'profit-negative': todayProfit < 0 }">
            {{ todayProfit.toFixed(2) }}元
          </div>
        </div>
      </el-card>
    </div>
    
    <!-- 筛选条件 -->
    <div class="filter-container">
      <div class="filter-buttons">
        <span class="filter-label">统计类型</span>
          <el-radio-group v-model="statsType" @change="loadFinancialData">
            <el-radio-button value="income">收入</el-radio-button>
            <el-radio-button value="expense">支出</el-radio-button>
            <el-radio-button value="profit">利润</el-radio-button>
          </el-radio-group>
        
        <span class="filter-label">时间维度</span>
          <el-radio-group v-model="timeDimension" @change="loadFinancialData">
            <el-radio-button value="day">日</el-radio-button>
            <el-radio-button value="month">月</el-radio-button>
            <el-radio-button value="year">年</el-radio-button>
          </el-radio-group>
      </div>
    </div>
    
    <!-- 图表 -->
    <div class="chart-container">
      <el-card>
        <div id="statsChart" ref="chartRef" class="chart"></div>
      </el-card>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, watch, computed } from 'vue'
import axios from 'axios'
import * as echarts from 'echarts'
import { ElMessage } from 'element-plus'

// 数据
const todayIncome = ref(0)
const todayExpense = ref(0)
const todayProfit = ref(0)
const statsType = ref('profit') // 默认显示利润
const timeDimension = ref('day') // 默认按日查看
const chartRef = ref(null)
let chart = null

// 获取用户信息
const getUser = () => JSON.parse(localStorage.getItem('user'))

// 计算今日利润
const calculateTodayProfit = () => {
  todayProfit.value = todayIncome.value - todayExpense.value
}

// 加载财务数据
const loadFinancialData = async () => {
  const user = getUser()
  if (!user || !user.user_id) {
    ElMessage.error('用户未登录')
    return
  }
  
  try {
    const res = await axios.get(
      'http://192.168.56.100:8080/financialStatistics',
      {
        params: {
          user_id: user.user_id,
          stats_type: statsType.value,
          time_dimension: timeDimension.value
        }
      }
    )
    
    if (res.data.code === 0) {
      // 更新今日数据
      todayIncome.value = res.data.data.today_income
      todayExpense.value = res.data.data.today_expense
      todayProfit.value = res.data.data.today_profit
      
      // 渲染图表
      renderChart(res.data.data.labels, res.data.data.values)
    } else {
      ElMessage.error(res.data.msg || '获取财务数据失败')
    }
  } catch (error) {
    console.error('获取财务数据失败:', error)
    ElMessage.error('网络错误，请稍后重试')
  }
}

// 渲染图表
const renderChart = (labels, values) => {
  if (!chartRef.value) return
  
  if (!chart) {
    chart = echarts.init(chartRef.value)
  }
  
  // 根据统计类型设置颜色
  let color = '#67C23A' // 默认绿色
  if (statsType.value === 'expense') {
    color = '#F56C6C' // 支出红色
  } else if (statsType.value === 'profit') {
    // 利润根据正负值设置颜色
    color = values.map(val => val >= 0 ? '#67C23A' : '#F56C6C')
  }
  
  const option = {
    title: {
      text: `${getStatsTypeText()}统计`,
      left: 'center'
    },
    tooltip: {
      trigger: 'axis',
      axisPointer: {
        type: 'shadow'
      },
      formatter: function(params) {
        return `${params[0].name}<br/>${getStatsTypeText()}: ${params[0].value.toFixed(2)}元`
      }
    },
    xAxis: {
      type: 'category',
      data: labels,
      axisLabel: {
        rotate: timeDimension.value === 'day' ? 45 : 0
      }
    },
    yAxis: {
      type: 'value',
      name: '金额（元）'
    },
    series: [{
      data: values,
      type: 'bar',
      itemStyle: {
        color: color
      },
      label: {
        show: true,
        position: 'top',
        formatter: '{c}'
      }
    }]
  }
  
  chart.setOption(option)
}

// 获取统计类型文本
const getStatsTypeText = () => {
  switch (statsType.value) {
    case 'income': return '收入'
    case 'expense': return '支出'
    case 'profit': return '利润'
    default: return '统计'
  }
}

// 监听窗口大小变化，重新调整图表大小
const handleResize = () => {
  if (chart) {
    chart.resize()
  }
}

// 初始化
onMounted(() => {
  loadFinancialData()
  window.addEventListener('resize', handleResize)
})

// 组件卸载时清理
const onUnmounted = () => {
  if (chart) {
    chart.dispose()
  }
  window.removeEventListener('resize', handleResize)
}
</script>

<style scoped>
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
}

.income-card .card-value {
  color: #67C23A;
}

.expense-card .card-value {
  color: #F56C6C;
}

.profit-positive {
  color: #67C23A;
}

.profit-negative {
  color: #F56C6C;
}

.filter-container {
  margin-bottom: 20px;
}

.filter-buttons {
  display: flex;
  align-items: center;
  gap: 20px;
}

.filter-label {
  font-size: 14px;
  color: #606266;
  margin-right: 8px;
}

.el-radio-group {
  margin-right: 10px;
}

.chart-container {
  margin-top: 20px;
}

.chart {
  width: 100%;
  height: 400px;
}
</style>