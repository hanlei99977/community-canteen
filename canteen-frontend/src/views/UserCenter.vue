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

        <el-form-item label="地址">
          <el-input v-model="form.address" />
        </el-form-item>

        <el-form-item label="身份证号">
          <el-input v-model="form.idCard" />
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

const form = ref({
  user_id: '',
  username: '',
  age: '',
  phone: '',
  idCard: '',
  address: '',
  taste_preference: '',
  disease_history: '',
  family_id: '',   
  family_name: ''
})

const familyList = ref([])

// ✅ 获取用户信息（做映射）
const getUserInfo = async () => {
    const user = JSON.parse(localStorage.getItem('user'))
    const res = await axios.get('http://192.168.56.100:8080/userCenter', {
      params: {
        user_id: user.user_id
      }
  })

  const data = res.data.data

  // 🔥 关键：字段转换
  form.value = {
    user_id: data.user_id,
    username: data.username,
    age: data.age,
    phone: data.phone,
    address: data.address,

    taste_preference: data.tastePreference,
    disease_history: data.diseaseHistory,
    family_name: data.familyName,
    family_id: data.familyId,
  }
}

// 获取家庭列表
const getFamilyList = async () => {
  const res = await axios.get('http://192.168.56.100:8080/familyList')
  familyList.value = res.data.data
}

// 更新用户
const updateUser = async () => {
  await axios.post('/api/user/update', {
    user_id: form.value.user_id,
    age: form.value.age,
    phone: form.value.phone,
    address: form.value.address,
    taste_preference: form.value.taste_preference,
    family_id: form.value.family_id
  })

  ElMessage.success('更新成功')
}

onMounted(() => {
  getFamilyList()
  getUserInfo()
})
</script>

<style scoped>
.user-center {
  padding: 20px;
}
</style>