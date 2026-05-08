<template>
  <div>
    <h2>食堂列表</h2>

    <el-table :data="canteens" style="width: 100%">
      <el-table-column prop="id" label="ID" width="100" />
      <el-table-column label="食堂名称" min-width="220">
        <template #default="scope">
          <span>{{ scope.row.name }}</span>
          <el-tag size="small" style="margin-left: 10px" :type="scope.row.rating_count >= 20 ? 'success' : 'info'">
            {{ scoreText(scope.row) }}
          </el-tag>
        </template>
      </el-table-column>
      <el-table-column prop="rating_count" label="评分数量" width="120" />

      <el-table-column label="操作" width="440">
        <template #default="scope">
          <el-button type="primary" @click="viewMenu(scope.row)">
            查看菜单
          </el-button>
          <el-button @click="viewReviews(scope.row)">
            查看评价
          </el-button>
          <el-button type="danger" plain @click="openComplaintDialog(scope.row)">
            投诉
          </el-button>
          <el-button @click="openMessageDialog(scope.row)">
            留言
          </el-button>
        </template>
      </el-table-column>
    </el-table>

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

        <!-- 历史留言 -->
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
import { ref, onMounted } from 'vue'
import axios from 'axios'
import { useRouter } from 'vue-router'
import { ElMessage } from 'element-plus'

const router = useRouter()
const canteens = ref([])
const reviewDialogVisible = ref(false)
const activeCanteen = ref(null)
const complaintDialogVisible = ref(false)
const complaintTarget = ref(null)
const complaintForm = ref({
  type: 1,
  content: ''
})

// 留言相关
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

// 获取食堂列表
onMounted(async () => {
  const res = await axios.get('http://192.168.56.100:8080/canteens')
  canteens.value = res.data.data || []
})

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

// 留言相关方法
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
