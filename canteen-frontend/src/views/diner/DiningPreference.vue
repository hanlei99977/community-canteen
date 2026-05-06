<template>
  <div>
    <h2>用餐偏好</h2>

    <!-- 图表区域 -->
    <div class="chart-container">
      <el-card class="chart-card">
        <template #header>
          <div class="card-header">
            <span>用餐偏好</span>
          </div>
        </template>
        <div v-if="tagPreference.length === 0" class="empty-tip">暂无用餐偏好数据</div>
        <div v-else id="tagChart" ref="tagChartRef" class="chart"></div>
      </el-card>
      <el-card class="chart-card">
        <template #header>
          <div class="card-header">
            <span>菜品购买排行榜</span>
            <el-radio-group v-model="timeDimension" @change="loadDishPurchaseRanking" size="small">
              <el-radio-button label="day">今天</el-radio-button>
              <el-radio-button label="week">最近7天</el-radio-button>
              <el-radio-button label="month">最近30天</el-radio-button>
            </el-radio-group>
          </div>
        </template>
        <div v-if="rankingData.length === 0" class="empty-tip">当前时间段内暂无购买记录</div>
        <div v-else id="rankingChart" ref="rankingChartRef" class="chart"></div>
      </el-card>
    </div>

    <!-- 菜品详情弹窗 -->
    <el-dialog v-model="dishDetailVisible" title="菜品详情" width="500px">
      <div v-if="dishDetail" class="dish-detail">
        <el-descriptions :column="1" border>
          <el-descriptions-item label="菜品名称">{{ dishDetail.dish_name }}</el-descriptions-item>
          <el-descriptions-item label="所属食堂">{{ dishDetail.canteen_name }}</el-descriptions-item>
          <el-descriptions-item label="价格">{{ dishDetail.price }}元</el-descriptions-item>
          <el-descriptions-item label="卡路里">{{ dishDetail.calories }}kcal</el-descriptions-item>
          <el-descriptions-item label="营养信息">{{ dishDetail.nutrition_info || '暂无' }}</el-descriptions-item>
          <el-descriptions-item label="标签">
            <el-tag v-for="tag in dishDetail.tags" :key="tag" size="small" style="margin-right: 5px">
              {{ tag }}
            </el-tag>
            <span v-if="!dishDetail.tags || dishDetail.tags.length === 0">暂无</span>
          </el-descriptions-item>
        </el-descriptions>
      </div>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted, nextTick } from 'vue'
import axios from 'axios'
import * as echarts from 'echarts'
import { ElMessage } from 'element-plus'

const timeDimension = ref('month')
const tagChartRef = ref(null)
const rankingChartRef = ref(null)
const dishDetailVisible = ref(false)
const dishDetail = ref(null)
const rankingData = ref([])
const tagPreference = ref([])
let tagChart = null
let rankingChart = null

const getUser = () => JSON.parse(localStorage.getItem('user'))

const loadDiningPreference = async () => {
  const user = getUser()
  if (!user || !user.user_id) {
    ElMessage.error('用户未登录')
    return
  }

  try {
    const res = await axios.get('http://192.168.56.100:8080/diningPreference', {
      params: {
        user_id: user.user_id,
        time_dimension: 'month'
      }
    })

    if (res.data.code === 0) {
      tagPreference.value = res.data.data.tag_preference || []
      await nextTick()
      renderTagChart(res.data.data.tag_preference)
    } else {
      ElMessage.error(res.data.msg || '获取用餐偏好数据失败')
    }
  } catch (error) {
    console.error('获取用餐偏好数据失败:', error)
    ElMessage.error('网络错误，请稍后重试')
  }
}

const loadDishPurchaseRanking = async () => {
  const user = getUser()
  if (!user || !user.user_id) {
    return
  }

  try {
    const res = await axios.get('http://192.168.56.100:8080/dishPurchaseRanking', {
      params: {
        user_id: user.user_id,
        time_dimension: timeDimension.value
      }
    })

    if (res.data.code === 0) {
      rankingData.value = res.data.data
      await nextTick()
      renderRankingChart(res.data.data)
    }
  } catch (error) {
    console.error('获取菜品购买排行榜失败:', error)
  }
}

const loadDishDetail = async (dishId) => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/dishDetail', {
      params: {
        dish_id: dishId
      }
    })

    if (res.data.code === 0) {
      dishDetail.value = res.data.data
      dishDetailVisible.value = true
    }
  } catch (error) {
    console.error('获取菜品详情失败:', error)
    ElMessage.error('获取菜品详情失败')
  }
}

const renderTagChart = (tagPreference) => {
  if (!tagChartRef.value) return

  if (tagChart && (!tagChart._dom || tagChart._dom !== tagChartRef.value)) {
    tagChart.dispose()
    tagChart = null
  }

  if (!tagChart) {
    tagChart = echarts.init(tagChartRef.value)
  }

  const data = tagPreference.map(item => ({
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
      data: tagPreference.map(item => item.name)
    },
    series: [
      {
        name: '用餐偏好',
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

  tagChart.setOption(option)
}

const renderRankingChart = (rankingDataParam) => {
  if (!rankingChartRef.value) return

  if (rankingChart && (!rankingChart._dom || rankingChart._dom !== rankingChartRef.value)) {
    rankingChart.dispose()
    rankingChart = null
  }

  if (!rankingChart) {
    rankingChart = echarts.init(rankingChartRef.value)
  }

  const sortedData = [...rankingDataParam].sort((a, b) => b.quantity - a.quantity)

  const option = {
    tooltip: {
      trigger: 'axis',
      axisPointer: {
        type: 'shadow'
      },
      formatter: (params) => {
        const item = params[0]
        const data = sortedData[item.dataIndex]
        return `${data.dish_name}<br/>食堂: ${data.canteen_name}<br/>购买数量: ${data.quantity}<br/><span style="color: #409EFF">点击查看菜品详情</span>`
      }
    },
    grid: {
      left: '3%',
      right: '4%',
      bottom: '3%',
      top: '3%',
      containLabel: true
    },
    xAxis: {
      type: 'value'
    },
    yAxis: {
      type: 'category',
      inverse: true,
      data: sortedData.map(item => item.dish_name),
      axisLabel: {
        interval: 0,
        rotate: 0,
        formatter: (value) => {
          const data = sortedData.find(item => item.dish_name === value)
          return value.length > 10 ? value.substring(0, 10) + '...' : value
        }
      }
    },
    series: [
      {
        name: '购买数量',
        type: 'bar',
        data: sortedData.map((item, index) => ({
          value: item.quantity,
          dishId: item.dish_id,
          itemStyle: {
            color: ['#5470c6', '#91cc75', '#fac858', '#ee6666', '#73c0de', '#3ba272', '#fc8452', '#9a60b4', '#ea7ccc', '#5470c6', '#91cc75', '#fac858', '#ee6666', '#73c0de', '#3ba272'][index % 15]
          }
        })),
        label: {
          show: true,
          position: 'right',
          formatter: (params) => {
            const data = sortedData[params.dataIndex]
            return `${data.quantity} (${data.canteen_name})`
          }
        }
      }
    ]
  }

  rankingChart.off('click')
  rankingChart.on('click', (params) => {
    const dataIndex = params.dataIndex
    const dishId = sortedData[dataIndex]?.dish_id
    if (dishId) {
      loadDishDetail(dishId)
    }
  })

  rankingChart.setOption(option)
}

const handleResize = () => {
  if (tagChart) {
    tagChart.resize()
  }
  if (rankingChart) {
    rankingChart.resize()
  }
}

onMounted(() => {
  loadDiningPreference()
  loadDishPurchaseRanking()
  window.addEventListener('resize', handleResize)
})

onUnmounted(() => {
  if (tagChart) {
    tagChart.dispose()
  }
  if (rankingChart) {
    rankingChart.dispose()
  }
  window.removeEventListener('resize', handleResize)
})
</script>

<style scoped>
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

.empty-tip {
  width: 100%;
  height: 400px;
  display: flex;
  justify-content: center;
  align-items: center;
  color: #909399;
  font-size: 14px;
}

.dish-detail {
  padding: 10px;
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
