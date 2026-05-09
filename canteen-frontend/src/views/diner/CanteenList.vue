<template>
  <div class="canteen-container">
    <div class="page-header">
      <h2>社区长者食堂</h2>
      <p class="subtitle">为您提供优质的就餐服务</p>
    </div>

    <!-- 区域选择 -->
    <div class="region-filter">
      <div class="city-filter">
        <el-button 
          v-for="city in cities" 
          :key="city.name"
          :class="['city-btn', { active: selectedCity === city.name }]"
          @click="selectCity(city.name)"
        >
          {{ city.name }}
        </el-button>
      </div>
      <div class="district-filter" v-if="selectedCity">
        <el-button 
          v-for="district in currentDistricts" 
          :key="district"
          :class="['district-btn', { active: selectedDistrict === district }]"
          @click="selectedDistrict = district"
        >
          {{ district }}
        </el-button>
      </div>
    </div>

    <!-- 按区域分组显示食堂卡片 -->
    <div v-for="(canteensInRegion, region) in groupedCanteens" :key="region" class="region-section">
      <div class="region-header">
        <div class="region-icon">📍</div>
        <h3>{{ region }}</h3>
        <span class="canteen-count">{{ canteensInRegion.length }} 家食堂</span>
      </div>
      
      <div class="canteen-grid">
        <el-card 
          v-for="canteen in canteensInRegion" 
          :key="canteen.id" 
          class="canteen-card"
          hover
        >
          <div class="canteen-header">
            <h4 class="canteen-name">{{ canteen.name }}</h4>
            <el-tag 
              :type="canteen.rating_count >= 20 ? 'success' : 'info'" 
              size="small"
            >
              {{ scoreText(canteen) }}
            </el-tag>
          </div>
          
          <div class="canteen-info">
            <div class="info-item">
              <span class="info-icon">🏠</span>
              <span>{{ canteen.address || '暂无地址信息' }}</span>
            </div>
          </div>

          <div class="canteen-actions">
            <el-button type="primary" size="small" @click="viewMenu(canteen)">
              <span>🍽️</span> 查看菜单
            </el-button>
            <el-button size="small" @click="viewReviews(canteen)">
              <span>⭐</span> 评价({{ canteen.rating_count }})
            </el-button>
            <el-button size="small" @click="openMessageDialog(canteen)">
              <span>💬</span> 留言
            </el-button>
            <el-button type="danger" size="small" plain @click="openComplaintDialog(canteen)">
              <span>📝</span> 投诉
            </el-button>
          </div>
        </el-card>
      </div>
    </div>

    <!-- 空状态 -->
    <div v-if="filteredCanteens.length === 0" class="empty-state">
      <el-empty description="该区域暂无食堂" />
    </div>

    <!-- 评价详情弹窗 -->
    <el-dialog v-model="reviewDialogVisible" title="食堂评价详情" width="700px">
      <div v-if="activeCanteen">
        <div style="margin-bottom: 12px; font-weight: 600;">
          {{ activeCanteen.name }}（{{ scoreText(activeCanteen) }}）
        </div>

        <el-empty v-if="!activeCanteen.reviews || activeCanteen.reviews.length === 0" description="暂无评价" />

        <el-table v-else :data="activeCanteen.reviews" border>
          <el-table-column prop="username" label="评价人" width="100" />
          <el-table-column prop="score" label="评分" width="80" />
          <el-table-column prop="comment" label="评价内容" min-width="220" />
          <el-table-column label="下单菜品" min-width="200">
            <template #default="scope">
              {{ (scope.row.dishes || []).join('、') || '无菜品信息' }}
            </template>
          </el-table-column>
          <el-table-column prop="time" label="评价时间" width="180" />
        </el-table>
      </div>
    </el-dialog>

    <!-- 投诉弹窗 -->
    <el-dialog v-model="complaintDialogVisible" title="投诉食堂" width="520px">
      <el-form label-width="90px">
        <el-form-item label="食堂">
          <span>{{ complaintTarget?.name }}</span>
        </el-form-item>
        <el-form-item label="投诉类型">
          <el-select v-model="complaintForm.type" placeholder="请选择投诉类型" style="width: 100%">
            <el-option label="食品安全问题" :value="1" />
            <el-option label="服务态度问题" :value="2" />
            <el-option label="环境卫生问题" :value="3" />
            <el-option label="其他" :value="4" />
          </el-select>
        </el-form-item>
        <el-form-item label="投诉内容">
          <el-input
            v-model="complaintForm.content"
            type="textarea"
            :rows="4"
            maxlength="300"
            show-word-limit
            placeholder="请填写投诉内容"
          />
        </el-form-item>
      </el-form>

      <template #footer>
        <el-button @click="complaintDialogVisible = false">取消</el-button>
        <el-button type="primary" @click="submitComplaint">提交投诉</el-button>
      </template>
    </el-dialog>

    <!-- 留言弹窗 -->
    <el-dialog v-model="messageDialogVisible" title="留言板" width="600px">
      <div v-if="messageTarget">
        <div style="margin-bottom: 12px; font-weight: 600;">
          {{ messageTarget.name }}
        </div>

        <div style="margin-bottom: 20px;">
          <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px;">
            <h4>历史留言</h4>
            <el-button type="primary" size="small" @click="openNewMessageDialog">新增</el-button>
          </div>
          
          <el-empty v-if="!messages.length" description="暂未留言" />
          
          <el-timeline v-else>
            <el-timeline-item
              v-for="message in messages"
              :key="message.id"
              :timestamp="message.message_time"
              :type="message.status === 1 ? 'success' : 'warning'"
            >
              <div style="padding: 10px; background: #f9f9f9; border-radius: 4px;">
                <div style="font-weight: 600; margin-bottom: 5px;">留言内容：</div>
                <div>{{ message.content }}</div>
                <div v-if="message.reply" style="margin-top: 10px; padding-top: 10px; border-top: 1px solid #eee;">
                  <div style="font-weight: 600; margin-bottom: 5px;">回复：</div>
                  <div>{{ message.reply }}</div>
                  <div style="margin-top: 5px; font-size: 12px; color: #999;">{{ message.reply_time }}</div>
                </div>
              </div>
            </el-timeline-item>
          </el-timeline>
        </div>
      </div>

      <template #footer>
        <el-button @click="messageDialogVisible = false">关闭</el-button>
      </template>
    </el-dialog>

    <!-- 新增留言弹窗 -->
    <el-dialog v-model="newMessageDialogVisible" title="发布留言" width="520px">
      <el-form label-width="90px">
        <el-form-item label="食堂">
          <span>{{ messageTarget?.name }}</span>
        </el-form-item>
        <el-form-item label="留言内容">
          <el-input
            v-model="newMessageForm.content"
            type="textarea"
            :rows="4"
            maxlength="300"
            show-word-limit
            placeholder="请填写留言内容"
          />
        </el-form-item>
      </el-form>

      <template #footer>
        <el-button @click="newMessageDialogVisible = false">取消</el-button>
        <el-button type="primary" @click="submitNewMessage">提交留言</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
import axios from 'axios'
import { useRouter } from 'vue-router'
import { ElMessage } from 'element-plus'

const router = useRouter()
const canteens = ref([])
const selectedCity = ref('沈阳市')
const selectedDistrict = ref('全部')

// 城市与区域映射
const cities = ref([
  { name: '沈阳市', districts: ['全部', '和平区', '大东区', '皇姑区', '铁西区', '苏家屯区', '浑南区', '沈北新区'] },
  { name: '大连市', districts: ['全部', '沙河口区', '中山区', '西岗区', '甘井子区', '旅顺口区', '金州区'] }
])

const reviewDialogVisible = ref(false)
const activeCanteen = ref(null)
const complaintDialogVisible = ref(false)
const complaintTarget = ref(null)
const complaintForm = ref({
  type: 1,
  content: ''
})

const messageDialogVisible = ref(false)
const newMessageDialogVisible = ref(false)
const messageTarget = ref(null)
const messages = ref([])
const newMessageForm = ref({
  content: ''
})

const scoreText = (canteen) => {
  if (!canteen || canteen.rating_count < 20) {
    return '评价过少'
  }
  return `评分 ${Number(canteen.avg_score).toFixed(1)}`
}

// 获取当前城市的区域列表
const currentDistricts = computed(() => {
  const city = cities.value.find(c => c.name === selectedCity.value)
  return city ? city.districts : []
})

// 选择城市时重置区域选择
const selectCity = (cityName) => {
  selectedCity.value = cityName
  selectedDistrict.value = '全部'
}

// 筛选后的食堂列表
const filteredCanteens = computed(() => {
  let result = canteens.value
  
  // 按城市筛选
  if (selectedCity.value === '沈阳市') {
    result = result.filter(c => c.region && 
      ['和平区', '大东区', '皇姑区', '铁西区', '苏家屯区', '浑南区', '沈北新区'].includes(c.region))
  } else if (selectedCity.value === '大连市') {
    result = result.filter(c => c.region && 
      ['沙河口区', '中山区', '西岗区', '甘井子区', '旅顺口区', '金州区'].includes(c.region))
  }
  
  // 按区域筛选
  if (selectedDistrict.value !== '全部') {
    result = result.filter(c => c.region === selectedDistrict.value)
  }
  
  return result
})

// 按区域分组的食堂
const groupedCanteens = computed(() => {
  const filtered = filteredCanteens.value
  const grouped = {}
  
  filtered.forEach(canteen => {
    const region = canteen.region || '未分区'
    if (!grouped[region]) {
      grouped[region] = []
    }
    grouped[region].push(canteen)
  })
  
  return grouped
})

// 获取食堂列表
onMounted(async () => {
  const res = await axios.get('http://192.168.56.100:8080/canteens')
  canteens.value = res.data.data || []
  
  // 设置默认区域为用户所在区域
  setDefaultRegion()
})

// 设置默认区域为用户所在区域
const setDefaultRegion = () => {
  try {
    const userInfo = localStorage.getItem('userInfo')
    if (userInfo) {
      const user = JSON.parse(userInfo)
      const userRegion = user.region || user.address || ''
      
      // 根据用户区域匹配城市和区域
      if (userRegion.includes('沈阳') || userRegion.includes('和平') || userRegion.includes('大东') || 
          userRegion.includes('皇姑') || userRegion.includes('铁西') || userRegion.includes('苏家屯') ||
          userRegion.includes('浑南') || userRegion.includes('沈北')) {
        selectedCity.value = '沈阳市'
        
        // 尝试匹配具体区域
        const districts = ['和平区', '大东区', '皇姑区', '铁西区', '苏家屯区', '浑南区', '沈北新区']
        for (const district of districts) {
          if (userRegion.includes(district.replace('区', ''))) {
            selectedDistrict.value = district
            return
          }
        }
        selectedDistrict.value = '全部'
      } else if (userRegion.includes('大连') || userRegion.includes('沙河口') || userRegion.includes('中山') ||
                 userRegion.includes('西岗') || userRegion.includes('甘井子') || userRegion.includes('旅顺口') ||
                 userRegion.includes('金州')) {
        selectedCity.value = '大连市'
        
        // 尝试匹配具体区域
        const districts = ['沙河口区', '中山区', '西岗区', '甘井子区', '旅顺口区', '金州区']
        for (const district of districts) {
          if (userRegion.includes(district.replace('区', ''))) {
            selectedDistrict.value = district
            return
          }
        }
        selectedDistrict.value = '全部'
      }
    }
  } catch (e) {
    console.error('获取用户区域信息失败:', e)
  }
}

// 查看菜单
const viewMenu = (canteen) => {
  router.push({
    path: '/menu',
    query: {
      canteen_id: canteen.id
    }
  })
}

const viewReviews = (canteen) => {
  activeCanteen.value = canteen
  reviewDialogVisible.value = true
}

const openComplaintDialog = (canteen) => {
  complaintTarget.value = canteen
  complaintForm.value = {
    type: 1,
    content: ''
  }
  complaintDialogVisible.value = true
}

const submitComplaint = async () => {
  if (!complaintTarget.value) {
    return
  }
  if (!complaintForm.value.content.trim()) {
    ElMessage.warning('请填写投诉内容')
    return
  }

  try {
    const user = JSON.parse(localStorage.getItem('user') || '{}')
    const res = await axios.post('http://192.168.56.100:8080/report', {
      user_id: user.user_id,
      canteen_id: complaintTarget.value.id,
      type: complaintForm.value.type,
      content: complaintForm.value.content.trim()
    })

    if (res.data.code === 0) {
      ElMessage.success('投诉提交成功')
      complaintDialogVisible.value = false
    } else {
      ElMessage.error(res.data.message || '投诉提交失败')
    }
  } catch (error) {
    ElMessage.error('投诉提交失败')
  }
}

const openMessageDialog = async (canteen) => {
  messageTarget.value = canteen
  await getMessages(canteen.id)
  messageDialogVisible.value = true
}

const getMessages = async (canteen_id) => {
  try {
    const user = JSON.parse(localStorage.getItem('user') || '{}')
    const res = await axios.get('http://192.168.56.100:8080/commentUser', {
      params: {
        user_id: user.user_id,
        canteen_id: canteen_id
      }
    })
    messages.value = res.data.data || []
  } catch (error) {
    console.error('获取留言失败:', error)
    messages.value = []
  }
}

const openNewMessageDialog = () => {
  newMessageForm.value = {
    content: ''
  }
  newMessageDialogVisible.value = true
}

const submitNewMessage = async () => {
  if (!messageTarget.value) {
    return
  }
  if (!newMessageForm.value.content.trim()) {
    ElMessage.warning('请填写留言内容')
    return
  }

  try {
    const user = JSON.parse(localStorage.getItem('user') || '{}')
    const res = await axios.post('http://192.168.56.100:8080/commentCreate', {
      canteen_id: messageTarget.value.id,
      user_id: user.user_id,
      content: newMessageForm.value.content.trim()
    })

    if (res.data.code === 0) {
      ElMessage.success('留言提交成功')
      newMessageDialogVisible.value = false
      await getMessages(messageTarget.value.id)
    } else {
      ElMessage.error(res.data.message || '留言提交失败')
    }
  } catch (error) {
    ElMessage.error('留言提交失败')
  }
}
</script>

<style scoped>
.canteen-container {
  padding: 20px;
  max-width: 1400px;
  margin: 0 auto;
}

.page-header {
  text-align: center;
  margin-bottom: 30px;
}

.page-header h2 {
  font-size: 28px;
  color: #333;
  margin: 0 0 10px 0;
}

.page-header .subtitle {
  color: #666;
  margin: 0;
}

.region-filter {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
  margin-bottom: 30px;
  padding: 15px;
  background: #f8f9fa;
  border-radius: 8px;
}

.filter-btn {
  padding: 8px 20px;
  border-radius: 20px;
  background: #fff;
  border: 1px solid #d9d9d9;
  transition: all 0.3s;
}

.filter-btn:hover {
  border-color: #409eff;
  color: #409eff;
}

.filter-btn.active {
  background: #409eff;
  border-color: #409eff;
  color: #fff;
}

.city-filter {
  display: flex;
  gap: 10px;
  margin-bottom: 15px;
}

.city-btn {
  padding: 8px 24px;
  font-size: 16px;
  font-weight: 600;
  border-radius: 8px;
  transition: all 0.3s;
}

.city-btn.active {
  background: linear-gradient(135deg, #409eff 0%, #67c23a 100%);
  border-color: transparent;
  color: #fff;
}

.district-filter {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  padding-left: 5px;
}

.district-btn {
  padding: 6px 16px;
  font-size: 14px;
  border-radius: 20px;
  transition: all 0.3s;
}

.district-btn.active {
  background: #409eff;
  border-color: #409eff;
  color: #fff;
}

.region-section {
  margin-bottom: 40px;
}

.region-header {
  display: flex;
  align-items: center;
  gap: 10px;
  margin-bottom: 20px;
  padding-bottom: 10px;
  border-bottom: 2px solid #409eff;
}

.region-icon {
  font-size: 20px;
}

.region-header h3 {
  font-size: 20px;
  color: #333;
  margin: 0;
}

.canteen-count {
  font-size: 14px;
  color: #666;
  background: #f0f0f0;
  padding: 2px 10px;
  border-radius: 10px;
}

.canteen-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(320px, 1fr));
  gap: 20px;
}

.canteen-card {
  height: 100%;
  display: flex;
  flex-direction: column;
  transition: transform 0.3s, box-shadow 0.3s;
}

.canteen-card:hover {
  transform: translateY(-5px);
  box-shadow: 0 10px 20px rgba(0, 0, 0, 0.1);
}

.canteen-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 15px;
}

.canteen-name {
  font-size: 18px;
  font-weight: 600;
  color: #333;
  margin: 0;
}

.canteen-info {
  flex: 1;
  margin-bottom: 15px;
}

.info-item {
  display: flex;
  align-items: center;
  gap: 8px;
  margin-bottom: 10px;
  font-size: 14px;
  color: #666;
}

.info-icon {
  font-size: 16px;
}

.canteen-actions {
  display: grid;
  grid-template-columns: 1fr 1fr;
  grid-template-rows: 1fr 1fr;
  gap: 6px;
}

.canteen-actions .el-button {
  width: 100%;
  height: 36px;
}

.empty-state {
  text-align: center;
  padding: 60px 0;
}

@media (max-width: 768px) {
  .canteen-grid {
    grid-template-columns: 1fr;
  }
  
  .canteen-actions .el-button {
    min-width: calc(50% - 4px);
  }
}
</style>
