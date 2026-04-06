<template>
  <div class="login-container">
    <el-card class="login-card">
      <h2 style="display:flex;align-items: center;gap: 15px;justify-content: center;">社区食堂登录</h2>

      <el-form :model="form">
        <el-form-item label="用户名">
          <el-input v-model="form.username" placeholder="请输入用户名" />
        </el-form-item>

        <el-form-item label="密码  ">
          <el-input v-model="form.password" type="password" placeholder="请输入密码" />
        </el-form-item>

        <el-form-item>
          <el-button type="primary" @click="handleLogin" style="display:flex;align-items: center;gap: 15px;justify-content: center;">登录</el-button>
        </el-form-item>

        <el-button type="text" @click="goRegister" style="display:flex;align-items: center;gap: 15px;justify-content: center;">
          没有账号？去注册
        </el-button>
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

// 登录
const handleLogin = async () => {

  // ✅ 1. 基本校验
  if (!form.username || !form.password) {
    ElMessage.warning('请输入用户名和密码')
    return
  }

  try {
    const res = await axios.post('http://192.168.56.100:8080/login', form)

    if (res.data.code === 0) {
      ElMessage.success('登录成功')

      const user = res.data.data  // ⭐ 后端返回的数据
      const role = user.role      // ⭐ 关键：角色

      // ✅ 2. 存用户信息
      localStorage.setItem('user', JSON.stringify(user))

      router.push('/home')

    } else {
      ElMessage.error('用户名或密码错误')
    }
  } catch (error) {
    ElMessage.error('请求失败，请检查后端')
  }
}


// 去注册
const goRegister = () => {
  router.push('/register')
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