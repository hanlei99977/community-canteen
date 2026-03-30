<template>
  <el-container style="height: 100vh">
    
    <!-- 左侧菜单 -->
    <el-aside width="200px" style="background: #2d3a4b">
      <h3 style="color: white; text-align: center; padding: 20px 0">
        社区食堂
      </h3>

      <el-menu
        router
        default-active="/home"
        background-color="#2d3a4b"
        text-color="#fff"
        active-text-color="#ffd04b"
      >
        <el-menu-item index="/home">
          <span>首页</span>
        </el-menu-item>

        <el-menu-item index="/canteens">
          <span>食堂列表</span>
        </el-menu-item>

        <el-menu-item index="/orders">
          <span>我的订单</span>
        </el-menu-item>
      </el-menu>
    </el-aside>

    <!-- 主体 -->
    <el-container>

      <!-- 顶部栏 -->
      <el-header style="background: #fff; display: flex; justify-content: space-between; align-items: center">
        
        <div>欢迎使用社区食堂系统</div>

        <div>
          <span style="margin-right: 20px">
            👤 {{ user.username }}
          </span>
          <el-button type="danger" size="small" @click="logout">
            退出登录
          </el-button>
        </div>

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

// 退出登录
const logout = () => {
  localStorage.removeItem('user')
  router.push('/login')
}
</script>