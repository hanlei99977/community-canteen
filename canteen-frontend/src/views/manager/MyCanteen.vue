<template>
  <div class="my-canteen">
    <el-card>
      <h2>我的食堂</h2>

      <el-form :model="canteenForm" label-width="100px">
        <el-form-item label="食堂名称">
          <el-input v-model="canteenForm.name" disabled />
        </el-form-item>

        <el-form-item label="详细地址">
          <el-input v-model="canteenForm.address" />
        </el-form-item>

        <el-form-item label="区域">
          <el-input v-model="canteenForm.regionName" disabled />
        </el-form-item>

        <el-form-item label="评分">
          <el-input v-model="canteenForm.rating" disabled />
        </el-form-item>

        <el-form-item label="投诉数量">
          <el-input v-model="canteenForm.complaintCount" disabled />
        </el-form-item>

        <el-form-item>
          <el-button type="primary" @click="updateCanteenAddress">
            更新地址
          </el-button>
        </el-form-item>
      </el-form>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

// 食堂信息表单
const canteenForm = ref({
  id: '',
  name: '',
  address: '',
  regionId: '',
  regionName: '',
  rating: '',
  complaintCount: ''
})

// 请求头自动携带 token
axios.interceptors.request.use(config => {
  const token = localStorage.getItem('token')
  if (token) {
    config.headers.Authorization = 'Bearer ' + token
  }
  return config
})

// 获取我的食堂信息
const getMyCanteen = async () => {
  try {
    const user = JSON.parse(localStorage.getItem('user'))
    if (!user || !user.user_id) {
      ElMessage.error('请先登录')
      return
    }

    const res = await axios.get('http://192.168.56.100:8080/myCanteen', {
      params: { user_id: user.user_id }
    })

    if (res.data.code === 0) {
      const canteenData = res.data.data
      canteenForm.value = {
        id: canteenData.id,
        name: canteenData.name,
        address: canteenData.address,
        regionId: canteenData.regionId,
        regionName: canteenData.regionName,
        rating: canteenData.rating || '暂无评分',
        complaintCount: canteenData.complaintCount || 0
      }
    } else {
      ElMessage.error('获取食堂信息失败：' + res.data.message)
    }
  } catch (err) {
    ElMessage.error('获取食堂信息失败：' + (err.response?.data?.message || '服务器异常'))
    console.error(err)
  }
}

// 更新食堂地址
const updateCanteenAddress = async () => {
  try {
    if (!canteenForm.value.address.trim()) {
      ElMessage.error('请输入详细地址')
      return
    }

    const res = await axios.post('http://192.168.56.100:8080/updateCanteenAddress', {
      canteen_id: canteenForm.value.id,
      address: canteenForm.value.address
    })

    if (res.data.code === 0) {
      ElMessage.success('地址更新成功')
    } else {
      ElMessage.error('地址更新失败：' + res.data.message)
    }
  } catch (err) {
    ElMessage.error('地址更新失败：' + (err.response?.data?.message || '服务器异常'))
    console.error(err)
  }
}

onMounted(() => {
  getMyCanteen()
})
</script>

<style scoped>
.my-canteen {
  padding: 20px;
}
</style>