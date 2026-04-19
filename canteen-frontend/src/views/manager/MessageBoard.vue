<template>
  <div>
    <h2>留言板</h2>

    <el-table :data="messages" style="width: 100%">
      <el-table-column prop="id" label="ID" width="100" />
      <el-table-column prop="user_id" label="留言人ID" width="120" />
      <el-table-column prop="content" label="留言内容" min-width="300" />
      <el-table-column prop="message_time" label="留言时间" width="180" />
      <el-table-column prop="reply" label="回复内容" min-width="300" />
      <el-table-column prop="reply_time" label="回复时间" width="180" />
      <el-table-column label="状态" width="100">
        <template #default="scope">
          <el-tag :type="scope.row.status === 1 ? 'success' : 'warning'">
            {{ scope.row.status === 1 ? '已回复' : '待回复' }}
          </el-tag>
        </template>
      </el-table-column>
      <el-table-column label="操作" width="100">
        <template #default="scope">
          <el-button type="primary" size="small" @click="openReplyDialog(scope.row)" :disabled="scope.row.status === 1">
            回复
          </el-button>
        </template>
      </el-table-column>
    </el-table>

    <!-- 回复弹窗 -->
    <el-dialog v-model="replyDialogVisible" title="回复留言" width="520px">
      <el-form label-width="90px">
        <el-form-item label="留言内容">
          <el-input
            v-model="replyForm.content"
            type="textarea"
            :rows="4"
            disabled
          />
        </el-form-item>
        <el-form-item label="回复内容">
          <el-input
            v-model="replyForm.reply"
            type="textarea"
            :rows="4"
            maxlength="300"
            show-word-limit
            placeholder="请填写回复内容"
          />
        </el-form-item>
      </el-form>

      <template #footer>
        <el-button @click="replyDialogVisible = false">取消</el-button>
        <el-button type="primary" @click="submitReply">提交回复</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

const messages = ref([])
const replyDialogVisible = ref(false)
const replyForm = ref({
  id: '',
  content: '',
  reply: ''
})

// 获取留言列表
onMounted(async () => {
  await getMessages()
})

const getMessages = async () => {
  try {
    const user = JSON.parse(localStorage.getItem('user') || '{}')
    const res = await axios.get('http://192.168.56.100:8080/canteenMessages', {
      params: {
        canteen_id: user.canteen_id
      }
    })
    // 按状态排序，未回复的在上方
    messages.value = (res.data.data || []).sort((a, b) => a.status - b.status)
  } catch (error) {
    console.error('获取留言失败:', error)
    messages.value = []
  }
}

const openReplyDialog = (message) => {
  replyForm.value = {
    id: message.id,
    content: message.content,
    reply: ''
  }
  replyDialogVisible.value = true
}

const submitReply = async () => {
  if (!replyForm.value.reply.trim()) {
    ElMessage.warning('请填写回复内容')
    return
  }

  try {
    const res = await axios.post('http://192.168.56.100:8080/messageReply', {
      id: replyForm.value.id,
      reply: replyForm.value.reply.trim()
    })

    if (res.data.code === 0) {
      ElMessage.success('回复成功')
      replyDialogVisible.value = false
      await getMessages()
    } else {
      ElMessage.error(res.data.message || '回复失败')
    }
  } catch (error) {
    ElMessage.error('回复失败')
  }
}
</script>
