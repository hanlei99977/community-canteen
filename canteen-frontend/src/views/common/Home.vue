<template>
  <div>
    <el-card>
      <template #header>
        <div style="font-size: 18px; font-weight: 600;">社区食堂公告</div>
      </template>

      <el-empty v-if="announcementList.length === 0" description="暂无公告" />

      <el-timeline v-else>
        <el-timeline-item
          v-for="item in announcementList"
          :key="item.announce_id"
          :timestamp="item.publish_time"
          placement="top"
        >
          <el-card>
            <h3 style="margin: 0 0 8px 0;">{{ item.title }}</h3>
            <div style="color: #606266; margin-bottom: 10px;">{{ item.content }}</div>
            <div style="font-size: 12px; color: #909399;">
              发布人：{{ item.publisher_name }}
            </div>
          </el-card>
        </el-timeline-item>
      </el-timeline>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

const announcementList = ref([])
onMounted(() => {
  loadAnnouncementList()
})

const loadAnnouncementList = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/announcementList')
    if (res.data.code === 0) {
      announcementList.value = res.data.data || []
    }
  } catch (error) {
    ElMessage.error('获取公告失败')
  }
}

</script>
