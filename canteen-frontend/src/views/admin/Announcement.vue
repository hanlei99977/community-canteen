<template>
  <div>
    <el-card>
      <template #header>
        <div style="font-weight: 600;">公告发布</div>
      </template>

      <el-form label-width="80px">
        <el-form-item label="标题">
          <el-input v-model="form.title" placeholder="请输入公告标题" maxlength="50" show-word-limit />
        </el-form-item>
        <el-form-item label="内容">
          <el-input
            v-model="form.content"
            type="textarea"
            :rows="4"
            placeholder="请输入公告内容"
            maxlength="1000"
            show-word-limit
          />
        </el-form-item>
        <el-form-item>
          <el-button type="primary" @click="publishAnnouncement">发布公告</el-button>
        </el-form-item>
      </el-form>
    </el-card>

    <el-card style="margin-top: 16px;">
      <template #header>
        <div style="font-weight: 600;">公告列表</div>
      </template>

      <el-table :data="announcementList" stripe>
        <el-table-column prop="title" label="标题" min-width="160" />
        <el-table-column prop="content" label="内容" min-width="260" show-overflow-tooltip />
        <el-table-column prop="publisher_name" label="发布人" width="120" />
        <el-table-column prop="publish_time" label="发布时间" width="180" />
        <el-table-column label="操作" width="120">
          <template #default="scope">
            <el-button
              v-if="scope.row.publisher_id === currentUser.user_id"
              type="danger"
              size="small"
              @click="deleteAnnouncement(scope.row)"
            >
              删除
            </el-button>
            <span v-else style="color: #999;">无权限</span>
          </template>
        </el-table-column>
      </el-table>
    </el-card>
  </div>
</template>

<script setup>
import { onMounted, reactive, ref } from 'vue'
import axios from 'axios'
import { ElMessage, ElMessageBox } from 'element-plus'

const currentUser = ref(JSON.parse(localStorage.getItem('user') || '{}'))

const form = reactive({
  title: '',
  content: ''
})

const announcementList = ref([])

const loadAnnouncementList = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/announcementList')
    if (res.data.code === 0) {
      announcementList.value = res.data.data || []
    }
  } catch (error) {
    ElMessage.error('获取公告列表失败')
  }
}

const publishAnnouncement = async () => {
  if (!form.title.trim() || !form.content.trim()) {
    ElMessage.warning('请填写公告标题和内容')
    return
  }

  try {
    const res = await axios.post('http://192.168.56.100:8080/announcementPublish', {
      title: form.title.trim(),
      content: form.content.trim(),
      publisher_id: currentUser.value.user_id
    })

    if (res.data.code === 0) {
      ElMessage.success('公告发布成功')
      form.title = ''
      form.content = ''
      loadAnnouncementList()
    } else {
      ElMessage.error(res.data.message || '公告发布失败')
    }
  } catch (error) {
    ElMessage.error('公告发布失败')
  }
}

const deleteAnnouncement = async (row) => {
  try {
    await ElMessageBox.confirm(`确认删除公告《${row.title}》吗？`, '提示', {
      type: 'warning'
    })

    const res = await axios.post('http://192.168.56.100:8080/announcementDelete', {
      announce_id: row.announce_id,
      publisher_id: currentUser.value.user_id
    })

    if (res.data.code === 0) {
      ElMessage.success('删除成功')
      loadAnnouncementList()
    } else {
      ElMessage.error(res.data.message || '删除失败')
    }
  } catch (error) {
    if (error !== 'cancel') {
      ElMessage.error('删除失败')
    }
  }
}

onMounted(() => {
  loadAnnouncementList()
})
</script>
