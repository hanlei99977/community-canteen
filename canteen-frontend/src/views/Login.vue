<template>
  <div class="login-container">
    <el-card class="login-card">
      <h2>社区食堂登录</h2>

      <el-form :model="form">
        <el-form-item label="用户名">
          <el-input v-model="form.username" placeholder="请输入用户名" />
        </el-form-item>

        <el-form-item label="密码">
          <el-input v-model="form.password" type="password" placeholder="请输入密码" />
        </el-form-item>

        <el-form-item>
          <el-button type="primary" @click="handleLogin">登录</el-button>
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
  password: ''
})

const handleLogin = async () => {
  try {
    const res = await axios.post('http://192.168.56.100:8080/login', form)

    if (res.data.code === 0) {
      ElMessage.success('登录成功')
      // ⭐ 存用户信息
      localStorage.setItem('user', JSON.stringify(res.data.data))
      // ⭐ 跳转首页（关键）
      router.push('/home')
    } else {
      ElMessage.error('用户名或密码错误')
    }
  } catch (error) {
    ElMessage.error('请求失败，请检查后端')
  }
}
</script>

<style scoped>
.login-container {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 100vh;
  background: #f5f5f5;
}

.login-card {
  width: 400px;
}
</style>