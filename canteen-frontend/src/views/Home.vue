<template>
  <el-container style="height: 100vh">
    <!-- 主体 -->
    <el-container>

      <!-- 顶部栏 -->
      <el-header style="background: #fff; display: flex; justify-content: space-between; align-items: center">
        <div>欢迎使用社区食堂系统</div>
      </el-header>

      <!-- 内容区 -->
      <el-main>
        <h2>这里是首页内容 🎉</h2>
      </el-main>

    </el-container>

  </el-container>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { useRouter } from 'vue-router'

const router = useRouter()
const user = ref({})

onMounted(() => {
  const u = localStorage.getItem('user')
  if (u) {
    user.value = JSON.parse(u)
  }
})

</script>