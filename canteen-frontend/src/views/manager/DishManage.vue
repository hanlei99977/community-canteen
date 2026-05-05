<template>
  <div>

    <!-- 销量统计卡片 -->
    <el-card class="sales-card" style="margin-bottom: 20px;">
      <template #header>
        <div class="card-header">
          <span>销量排行榜</span>
          <el-button type="text" @click="openSalesDialog">查看更多</el-button>
        </div>
      </template>
      <div ref="salesChartRef" style="height: 200px;"></div>
    </el-card>

    <!-- 标题 + 新建 -->
    <div style="display:flex;justify-content: space-between;align-items: center;margin-bottom: 20px;">
      <h2>菜品管理</h2>
      <el-button type="primary" @click="openAddDialog">新增菜品</el-button>
    </div>

    <!-- Tab -->
    <el-tabs v-model="activeTab">

      <el-tab-pane label="在售菜品" name="on"/>
      <el-tab-pane label="已下架菜品" name="off"/>

    </el-tabs>

    <!-- 表格 -->
    <el-table
      :data="activeTab === 'on' ? onDishList : offDishList"
      border
    >

      <el-table-column prop="name" label="菜名"/>
      <el-table-column prop="type" label="类型"/>
      <el-table-column prop="price" label="价格"/>
      <el-table-column prop="calories" label="热量"/>
      <el-table-column prop="nutrition_info" label="营养信息"/>
      <el-table-column label="标签" width="200">
        <template #default="scope">
          <el-tag
            v-for="tag in scope.row.tags"
            :key="tag.tag_id"
            size="small"
            style="margin-right: 5px;"
          >
            {{ tag.tag_name }}
          </el-tag>
        </template>
      </el-table-column>

      <el-table-column label="操作" width="200">
        <template #default="scope">

          <el-button
            type="primary"
            size="small"
            @click="openEditDialog(scope.row)"
          >
            修改
          </el-button>

          <el-button
            v-if="activeTab === 'on'"
            type="warning"
            size="small"
            @click="disableDish(scope.row.dish_id)"
          >
            下架
          </el-button>

          <el-button
            v-else
            type="success"
            size="small"
            @click="enableDish(scope.row.dish_id)"
          >
            上架
          </el-button>

        </template>
      </el-table-column>

    </el-table>

    <!-- 新增弹窗 -->
    <el-dialog v-model="addDialogVisible" title="新增菜品" width="600px">

      <el-form label-width="80px">

        <el-form-item label="菜名">
          <el-input v-model="addForm.name"/>
        </el-form-item>

        <el-form-item label="类型">
          <el-select v-model="addForm.type">
            <el-option label="荤" value="荤"/>
            <el-option label="素" value="素"/>
          </el-select>
        </el-form-item>

        <el-form-item label="价格">
          <el-input v-model="addForm.price" type="number"/>
        </el-form-item>

        <el-form-item label="热量">
          <el-input v-model="addForm.calories" type="number"/>
        </el-form-item>

        <el-form-item label="营养信息">
          <el-input v-model="addForm.nutrition_info"/>
        </el-form-item>

        <el-form-item label="标签">
          <div style="display: flex; flex-wrap: wrap; gap: 10px;">
            <el-button
              v-for="tag in allTags"
              :key="tag.tag_id"
              :type="addForm.tag_ids.includes(tag.tag_id) ? 'primary' : 'default'"
              size="small"
              @click="toggleAddTag(tag.tag_id)"
            >
              {{ tag.tag_name }}
            </el-button>
          </div>
        </el-form-item>

      </el-form>

      <template #footer>
        <el-button @click="addDialogVisible = false">取消</el-button>
        <el-button type="primary" @click="submitAdd">确定</el-button>
      </template>

    </el-dialog>

    <!-- 修改弹窗 -->
    <el-dialog v-model="editDialogVisible" title="修改菜品" width="600px">

      <el-form label-width="80px">

        <el-form-item label="菜名">
          <el-input v-model="editForm.name" disabled/>
        </el-form-item>

        <el-form-item label="类型">
          <el-select v-model="editForm.type" disabled>
            <el-option label="荤" value="荤"/>
            <el-option label="素" value="素"/>
          </el-select>
        </el-form-item>

        <el-form-item label="价格">
          <el-input v-model="editForm.price" type="number"/>
        </el-form-item>

        <el-form-item label="热量">
          <el-input v-model="editForm.calories" type="number"/>
        </el-form-item>

        <el-form-item label="营养信息">
          <el-input v-model="editForm.nutrition_info"/>
        </el-form-item>

        <el-form-item label="标签">
          <div style="display: flex; flex-wrap: wrap; gap: 10px;">
            <el-button
              v-for="tag in allTags"
              :key="tag.tag_id"
              :type="editForm.tag_ids.includes(tag.tag_id) ? 'primary' : 'default'"
              size="small"
              @click="toggleEditTag(tag.tag_id)"
            >
              {{ tag.tag_name }}
            </el-button>
          </div>
        </el-form-item>

      </el-form>

      <template #footer>
        <el-button @click="editDialogVisible = false">取消</el-button>
        <el-button type="primary" @click="submitEdit">确定</el-button>
      </template>

    </el-dialog>

    <!-- 销量排行弹窗 -->
    <el-dialog v-model="salesDialogVisible" title="销量排行榜" width="700px">
      <div style="margin-bottom: 20px;">
        <el-radio-group v-model="salesTimeRange" size="small" @change="handleTimeRangeChange">
          <el-radio label="today">今日</el-radio>
          <el-radio label="7days">近7天</el-radio>
          <el-radio label="30days">近30天</el-radio>
        </el-radio-group>
      </div>
      <div ref="salesDialogChartRef" style="height: 400px;"></div>
      <template #footer>
        <el-button @click="salesDialogVisible = false">关闭</el-button>
      </template>
    </el-dialog>

  </div>
</template>

<script setup>
import { ref, onMounted, computed, nextTick } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'
import * as echarts from 'echarts'

const getUser = () => JSON.parse(localStorage.getItem("user"))

const activeTab = ref("on")

const allDishList = ref([])
const allTags = ref([])

const onDishList = computed(() =>
  allDishList.value.filter(d => d.status === 1)
)

const offDishList = computed(() =>
  allDishList.value.filter(d => d.status === 0)
)

const addDialogVisible = ref(false)
const editDialogVisible = ref(false)
const salesDialogVisible = ref(false)
const salesTimeRange = ref('7days')
const salesChartRef = ref(null)
const salesDialogChartRef = ref(null)
const salesChart = ref(null)
const salesDialogChart = ref(null)
const salesData = ref([])

const addForm = ref({
  canteen_id: '',
  name: '',
  type: '',
  price: 0.0,
  calories: 0,
  nutrition_info: '',
  tag_ids: []
})

const editForm = ref({
  dish_id: '',
  name: '',
  type: '',
  price: 0.0,
  calories: 0,
  nutrition_info: '',
  tag_ids: []
})

const getAllTags = async () => {
  const res = await axios.get('http://192.168.56.100:8080/getAllTags')
  if (res.data.code === 0) {
    allTags.value = res.data.data
  }
}

const getDishTags = async (dish_id) => {
  const res = await axios.get('http://192.168.56.100:8080/getDishTags', {
    params: { dish_id }
  })
  if (res.data.code === 0) {
    return res.data.data.tag_ids || []
  }
  return []
}

const getDishes = async () => {
  const user = getUser()

  const res = await axios.get('http://192.168.56.100:8080/getDishes', {
    params: { canteen_id: user.canteen_id }
  })

  if (res.data.code === 0) {
    allDishList.value = res.data.data
    for (const dish of allDishList.value) {
      const tagRes = await axios.get('http://192.168.56.100:8080/getDishTags', {
        params: { dish_id: dish.dish_id }
      })
      if (tagRes.data.code === 0) {
        dish.tags = tagRes.data.data.tags || []
      } else {
        dish.tags = []
      }
    }
  }
}

const submitAdd = async () => {
  if (!addForm.value.name || !addForm.value.type || !addForm.value.price) {
    ElMessage.warning("请填写完整信息")
    return
  }

  addForm.value.price = Number(addForm.value.price)
  addForm.value.calories = parseInt(addForm.value.calories)

  try {
    const res = await axios.post(
      'http://192.168.56.100:8080/dishCreate',
      addForm.value
    )

    if (res.data.code === 0) {
      ElMessage.success("新增成功")
      addDialogVisible.value = false
      getDishes()
    }
  } catch (error) {
    ElMessage.error("新增失败")
  }
}

const submitEdit = async () => {
  editForm.value.price = Number(editForm.value.price)
  editForm.value.calories = parseInt(editForm.value.calories)

  try {
    const res = await axios.post(
      'http://192.168.56.100:8080/dishUpdate',
      {
        dish_id: editForm.value.dish_id,
        price: editForm.value.price,
        calories: editForm.value.calories,
        nutrition_info: editForm.value.nutrition_info,
        tag_ids: editForm.value.tag_ids
      }
    )

    if (res.data.code === 0) {
      ElMessage.success("修改成功")
      editDialogVisible.value = false
      getDishes()
    } else {
      ElMessage.error("修改失败")
    }
  } catch (error) {
    ElMessage.error("修改失败")
  }
}

const openAddDialog = () => {
  const user = getUser()

  addDialogVisible.value = true

  addForm.value = {
    canteen_id: user.canteen_id,
    name: '',
    type: '',
    price: 0.0,
    calories: 0,
    nutrition_info: '',
    tag_ids: []
  }
}

const openEditDialog = async (row) => {
  editDialogVisible.value = true

  const tag_ids = await getDishTags(row.dish_id)

  editForm.value = {
    dish_id: row.dish_id,
    name: row.name,
    type: row.type,
    price: row.price,
    calories: row.calories,
    nutrition_info: row.nutrition_info,
    tag_ids: tag_ids
  }
}

const toggleAddTag = (tag_id) => {
  const index = addForm.value.tag_ids.indexOf(tag_id)
  if (index === -1) {
    addForm.value.tag_ids.push(tag_id)
  } else {
    addForm.value.tag_ids.splice(index, 1)
  }
}

const toggleEditTag = (tag_id) => {
  const index = editForm.value.tag_ids.indexOf(tag_id)
  if (index === -1) {
    editForm.value.tag_ids.push(tag_id)
  } else {
    editForm.value.tag_ids.splice(index, 1)
  }
}

const disableDish = async (dish_id) => {
  const res = await axios.post('http://192.168.56.100:8080/dishDisable', { dish_id })
  if (res.data.code === 0) {
    ElMessage.success("已下架")
    getDishes()
  } else {
    ElMessage.error("该菜品在餐单中，不能下架")
  }
}

const enableDish = async (dish_id) => {
  await axios.post('http://192.168.56.100:8080/dishEnable', { dish_id })
  ElMessage.success("已上架")
  getDishes()
}

// 获取销量数据
const getSalesData = async (timeRange = '7days', limit = 3) => {
  const user = getUser()
  try {
    const res = await axios.get('http://192.168.56.100:8080/dishSales', {
      params: {
        canteen_id: user.canteen_id,
        time_range: timeRange,
        limit: limit
      }
    })
    if (res.data.code === 0) {
      return res.data.data
    }
  } catch (error) {
    console.error('获取销量数据失败:', error)
  }
  return []
}

// 渲染顶部销量图表（显示前3名）
const renderSalesChart = async () => {
  await nextTick()
  if (!salesChartRef.value) return
  
  salesData.value = await getSalesData('7days', 3)
  
  if (salesChart.value) {
    salesChart.value.dispose()
  }
  
  salesChart.value = echarts.init(salesChartRef.value)
  
  const option = {
    tooltip: {
      trigger: 'axis',
      axisPointer: {
        type: 'shadow'
      }
    },
    grid: {
      left: '15%',
      right: '15%',
      bottom: '3%',
      containLabel: true
    },
    xAxis: {
      type: 'value',
      name: '销量'
    },
    yAxis: {
      type: 'category',
      data: salesData.value.length > 0 ? salesData.value.map(item => item.name) : ['暂无数据'],
      inverse: true
    },
    series: [
      {
        name: '销量',
        type: 'bar',
        data: salesData.value.length > 0 ? salesData.value.map(item => ({
          value: item.sales,
          label: {
            show: true,
            position: 'right',
            formatter: '{c}份'
          }
        })) : [0],
        barWidth: '60%'
      }
    ]
  }
  
  salesChart.value.setOption(option)
}

// 渲染弹窗销量图表（显示前20名）
const renderSalesDialogChart = async () => {
  await nextTick()
  if (!salesDialogChartRef.value) return
  
  const data = await getSalesData(salesTimeRange.value, 15)
  
  if (salesDialogChart.value) {
    salesDialogChart.value.dispose()
  }
  
  salesDialogChart.value = echarts.init(salesDialogChartRef.value)
  
  const option = {
    tooltip: {
      trigger: 'axis',
      axisPointer: {
        type: 'shadow'
      }
    },
    grid: {
      left: '18%',
      right: '15%',
      bottom: '3%',
      containLabel: true
    },
    xAxis: {
      type: 'value',
      name: '销量'
    },
    yAxis: {
      type: 'category',
      data: data.length > 0 ? data.map(item => item.name) : ['暂无数据'],
      inverse: true
    },
    series: [
      {
        name: '销量',
        type: 'bar',
        data: data.length > 0 ? data.map(item => ({
          value: item.sales,
          label: {
            show: true,
            position: 'right',
            formatter: '{c}份'
          }
        })) : [0],
        barWidth: '50%'
      }
    ]
  }
  
  salesDialogChart.value.setOption(option)
}

// 打开销量弹窗
const openSalesDialog = async () => {
  salesDialogVisible.value = true
  salesTimeRange.value = '7days'
  await nextTick()
  renderSalesDialogChart()
}

// 时间范围改变时重新加载图表
const handleTimeRangeChange = () => {
  renderSalesDialogChart()
}

onMounted(() => {
  getAllTags()
  getDishes()
  renderSalesChart()
  
  window.addEventListener('resize', () => {
    salesChart.value?.resize()
    salesDialogChart.value?.resize()
  })
})
</script>
