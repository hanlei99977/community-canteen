<template>
  <div class="register-container">
    <el-card class="box-card">

      <h2 style="text-align:center;">用户注册</h2>

      <el-form :model="form">

        <el-form-item label="注册身份">
          <el-radio-group v-model="form.role">
            <el-radio label="diner">用餐者</el-radio>
            <el-radio label="manager">食堂管理者</el-radio>
            <el-radio label="admin">管理员</el-radio>
          </el-radio-group>
        </el-form-item>

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

        <template v-if="form.role === 'admin'">
          <el-form-item label="申请级别">
            <el-select v-model="form.level_id" placeholder="请选择管理员级别" style="width: 100%">
              <el-option label="市级管理员" :value="2" />
              <el-option label="区级管理员" :value="3" />
            </el-select>
          </el-form-item>

          <el-form-item label="管理区域">
            <el-select v-model="form.region_id" placeholder="请选择管理区域" style="width: 100%">
              <el-option
                v-for="region in regionOptions"
                :key="region.region_id"
                :label="region.region_name"
                :value="region.region_id"
              />
            </el-select>
          </el-form-item>
        </template>

        <template v-if="form.role === 'manager'">
          <el-form-item label="食堂名称">
            <el-input v-model="form.canteen_name" placeholder="请输入食堂名称" />
          </el-form-item>
        </template>

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
import { reactive, ref, onMounted } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'
import { useRouter } from 'vue-router'

const router = useRouter()

const form = reactive({
  username: '',
  password: '',
  age: 18,
  phone: '',
  role:'diner',
  level_id: 2,
  region_id: null,
  canteen_name: ''
})
const regionOptions = ref([])

onMounted(async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/regionList')
    if (res.data.code === 0) {
      regionOptions.value = res.data.data
    }
  } catch (e) {}
})

const handleRegister = async () => {
  if (form.role === 'admin' && !form.region_id) {
    ElMessage.warning('请选择管理区域')
    return
  }
  if (form.role === 'manager' && !form.canteen_name) {
    ElMessage.warning('请输入食堂名称')
    return
  }

  try {
    const res = await axios.post('http://192.168.56.100:8080/register', form,{ headers: { 'Content-Type': 'application/json' } })

    if (res.data.code === 0) {
      ElMessage.success(res.data.message || '注册成功')
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
