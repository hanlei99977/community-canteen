<template>
  <div class="user-center">
    <el-card>
      <h2>用户中心</h2>

      <el-form :model="form" label-width="100px">
        <el-form-item label="用户名">
          <el-input v-model="form.username" disabled />
        </el-form-item>

        <el-form-item label="年龄">
          <el-input v-model="form.age" />
        </el-form-item>

        <el-form-item label="手机号">
          <el-input v-model="form.phone" />
        </el-form-item>

        <el-form-item label="身份证号">
          <el-input v-model="form.id_card" />
        </el-form-item>

        <!-- 饮食习惯 -->
        <el-form-item label="饮食偏好">
          <el-select v-model="form.taste_preference" placeholder="请选择">
            <el-option label="清淡" value="清淡" />
            <el-option label="低糖" value="低糖" />
            <el-option label="低脂" value="低脂" />
            <el-option label="咸口" value="咸口" />
            <el-option label="辣口" value="辣口" />
          </el-select>
        </el-form-item>

        <!-- 疾病史 -->
        <el-form-item label="疾病史(如有)">
          <el-select v-model="form.disease_history" placeholder="请选择">
            <el-option label="无" value="无" />
            <el-option label="高血压" value="高血压" />
            <el-option label="糖尿病" value="糖尿病" />
          </el-select>
        </el-form-item>

        <!-- 家庭选择 -->
        <el-form-item label="所属家庭">
          <el-select v-model="form.family_id" placeholder="请选择家庭">
            <el-option
              v-for="item in familyList"
              :key="item.family_id"
              :label="item.family_name + ' (ID:' + item.family_id + ')'"
              :value="item.family_id"
            />
          </el-select>
        </el-form-item>

        <!-- 区域选择 -->
        <el-form-item label="所属区域">
          <el-select v-model="form.region_id" placeholder="请选择区域">
            <el-option
              v-for="item in regionList"
              :key="item.region_id"
              :label="item.region_name + ' (ID:' + item.region_id + ')'"
              :value="item.region_id"
            />
          </el-select>
        </el-form-item>

        <el-form-item>
          <el-button type="primary" @click="updateUser">
            保存修改
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

// 🔥 修复1：完整初始化表单（补齐所有缺失字段）
const form = ref({
  user_id: '',
  username: '',
  age: '',
  phone: '',
  id_card: '',
  address: '',
  taste_preference: '',
  disease_history: '',
  family_id: '',
  family_name: '',
  region_id: '',
  region_name: '',
  region_level: '',
  parent_id: ''
})

const familyList = ref([])
// 🔥 修复2：缺失 regionList 定义
const regionList = ref([])

// 请求头自动携带 token（解决401）
axios.interceptors.request.use(config => {
  const token = localStorage.getItem('token')
  if (token) {
    config.headers.Authorization = 'Bearer ' + token
  }
  return config
})

// ✅ 获取用户信息
const getUserInfo = async () => {
  try {
    // 🔥 修复3：判断登录状态，防止崩溃
    const user = JSON.parse(localStorage.getItem('user'))
    if (!user || !user.user_id) {
      ElMessage.error('请先登录')
      return
    }

    const res = await axios.get('http://192.168.56.100:8080/userCenter', {
      params: { user_id: user.user_id }
    })

    const data = res.data.data

    // 🔥 修复4：补齐所有字段映射（id_card、region等）
    form.value = {
      user_id: data.user_id,
      username: data.username,
      age: data.age,
      phone: data.phone,
      id_card: data.id_card, // 缺失
      address: data.address,
      taste_preference: data.tastePreference,
      disease_history: data.diseaseHistory,
      region_id: data.regionId,
      region_name: data.regionName,
      family_id: data.familyId,
      family_name: data.familyName
    }
  } catch (err) {
    ElMessage.error('获取用户信息失败')
    console.error(err)
  }
}

// 获取家庭列表
const getFamilyList = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/familyList')
    familyList.value = res.data.data || []
  } catch (err) {
    ElMessage.error('获取家庭列表失败')
  }
}

// 获取区域列表
const getRegionList = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/regionList')
    regionList.value = res.data.data || []
  } catch (err) {
    ElMessage.error('获取区域失败')
  }
}

// 更新用户
const updateUser = async () => {
  try {
    await axios.post('http://192.168.56.100:8080/userCenterUpdate', {
      user_id: form.value.user_id,
      age: form.value.age,
      phone: form.value.phone,
      address: form.value.address,
      id_card: form.value.id_card,
      taste_preference: form.value.taste_preference,
      disease_history: form.value.disease_history,
      family_id: form.value.family_id,
      region_id: form.value.region_id
    })

    ElMessage.success('更新成功')
  } catch (err) {
    // 🔥 修复5：异常捕获
    ElMessage.error('更新失败：' + (err.response?.data?.msg || '服务器异常'))
    console.error(err)
  }
}

onMounted(() => {
  // 🔥 修复6：补齐区域接口调用
  getFamilyList()
  getRegionList()
  getUserInfo()
})
</script>

<style scoped>
.user-center {
  padding: 20px;
}
</style>