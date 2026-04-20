<template>
  <div class="message-center">
    <h2>消息中心</h2>
    <el-card v-for="message in messages" :key="message.message_id" class="message-card" @click="markAsRead(message.message_id)">
      <div class="message-header">
        <span class="message-time">{{ message.create_time }}</span>
        <el-tag v-if="message.status === 0" type="danger" size="small">未读</el-tag>
        <el-tag v-else type="success" size="small">已读</el-tag>
      </div>
      <div class="message-content">{{ message.content }}</div>
    </el-card>
    <el-empty v-if="messages.length === 0" description="暂无消息" />
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

const messages = ref([])

const getMessages = async () => {
  try {
    const user = JSON.parse(localStorage.getItem('user'))
    const res = await axios.get('http://192.168.56.100:8080/messages', {
      params: { user_id: user.user_id }
    })
    if (res.data.code === 0) {
      messages.value = res.data.data
    } else {
      ElMessage.error(res.data.message)
    }
  } catch (error) {
    console.error('获取消息失败', error)
    ElMessage.error('获取消息失败')
  }
}

const markAsRead = async (message_id) => {
  try {
    const res = await axios.post('http://192.168.56.100:8080/markMessageAsRead', {
      message_id
    })
    if (res.data.code === 0) {
      // 更新本地消息状态
      const message = messages.value.find(m => m.message_id === message_id)
      if (message) {
        message.status = 1
      }
      ElMessage.success('消息已标记为已读')
    } else {
      ElMessage.error(res.data.message)
    }
  } catch (error) {
    console.error('标记消息为已读失败', error)
    ElMessage.error('标记消息为已读失败')
  }
}

onMounted(() => {
  getMessages()
})
</script>

<style scoped>
.message-center {
  padding: 20px;
}

.message-card {
  margin-bottom: 15px;
  cursor: pointer;
  transition: all 0.3s;
}

.message-card:hover {
  box-shadow: 0 2px 12px 0 rgba(0, 0, 0, 0.1);
}

.message-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 10px;
}

.message-time {
  font-size: 12px;
  color: #999;
}

.message-content {
  font-size: 14px;
  line-height: 1.5;
}
</style>