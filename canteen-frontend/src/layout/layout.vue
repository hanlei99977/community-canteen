<template>
  <el-container style="height: 100vh">

    <!-- 左侧菜单 -->
    <el-aside width="200px" style="background: #2d3a4b">
      <h3 style="color: white; text-align: center; padding: 20px 0">
        社区食堂
      </h3>

      <el-menu
        router
        :default-active="$route.path"
        background-color="#2d3a4b"
        text-color="#fff"
        active-text-color="#ffd04b"
      >
        <el-menu-item index="/home">首页</el-menu-item>
        <el-menu-item index="/canteens">食堂列表</el-menu-item>
        <el-menu-item index="/orders">我的订单</el-menu-item>
        <el-menu-item index="/userCenter">用户中心</el-menu-item>

      </el-menu>
    </el-aside>

    <!-- 右侧 -->
    <el-container>

      <!-- 顶部栏 -->
      <el-header style="display:flex;align-items: center;gap: 15px;justify-content: flex-end;">
          <span>{{ user.username }}</span>
          <el-button type="danger" size="small" @click="logout">
            退出
          </el-button>
      </el-header>

      <!-- ⭐ 关键：内容出口 -->
      <el-main>
        <router-view />
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
  if (u) user.value = JSON.parse(u)
})

const logout = () => {
  localStorage.removeItem('user')
  router.push('/login')
}
</script>