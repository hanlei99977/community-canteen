<template>
  <div class="register-container">
    <el-card class="box-card">

      <h2 style="text-align:center;">用户注册</h2>

      <el-form :model="form">

        <el-form-item label="用户名">
          <el-input v-model="form.username" />
        </el-form-item>

        <el-form-item label="密码">
          <el-input type="password" v-model="form.password" />
        </el-form-item>

        <el-form-item label="年龄">
          <el-input-number v-model="form.age" :min="0" :max="200" :step="1" controls-position="right"/>
        </el-form-item>

        <el-form-item label="手机号">
          <el-input v-model="form.phone" />
        </el-form-item>

        <el-form-item>
          <el-button type="primary" @click="handleRegister">
            注册
          </el-button>

          <el-button @click="goLogin">
            返回登录
          </el-button>
        </el-form-item>

      </el-form>

    </el-card>
  </div>
</template>

<script setup>
import { reactive } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'
import { useRouter } from 'vue-router'

const router = useRouter()

const form = reactive({
  username: '',
  password: '',
  age: 18,
  phone: ''
})

const handleRegister = async () => {
  try {
    const res = await axios.post('http://192.168.56.100:8080/register', form,{ headers: { 'Content-Type': 'application/json' } })

    if (res.data.code === 0) {
      ElMessage.success('注册成功')
      router.push('/login')
    } else {
      ElMessage.error(res.data.message || '注册失败')
    }

  } catch (error) {
    ElMessage.error('请求失败')
  }
}

const goLogin = () => {
  router.push('/login')
}
</script>

<style>
.register-container {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 100vh;
}
.box-card {
  width: 400px;
}
</style>