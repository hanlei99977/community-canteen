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
        <!-- ================== 用餐者专属 ================== -->
        <el-menu-item v-if="isDiner" index="/canteens">食堂列表</el-menu-item>
        <el-menu-item v-if="isDiner" index="/orders">我的订单</el-menu-item>
        <el-menu-item v-if="isDiner" index="/diningPreference">用餐偏好</el-menu-item>
        <el-menu-item v-if="isDiner" index="/messageCenter">消息中心</el-menu-item>

        <!-- ================== 管理员申请审核（系统管理员+市级管理员） ================== -->
        <el-menu-item v-if="canReviewAdminApply" index="/adminApply">
          管理员申请
        </el-menu-item>

        <!-- ================== 食堂管理者申请审核（系统管理员+市级管理员+区级管理员） ================== -->
        <el-menu-item v-if="canReviewManagerApply" index="/managerApply">
          食堂管理者申请
        </el-menu-item>

        <!-- ================== 管理员管理（系统管理员+市级管理员） ================== -->
        <el-menu-item v-if="canManageAdmin" index="/adminManage">
          管理员管理
        </el-menu-item>

        <!-- ================== 管理员通用功能（系统管理员+市级管理员+区级管理员） ================== -->
        <el-menu-item v-if="isAdmin" index="/dinerManage">
          用餐者管理
        </el-menu-item>

        <el-menu-item v-if="isAdmin" index="/canteenManage">
          食堂管理
        </el-menu-item>

        <el-menu-item v-if="isAdmin" index="/announcement">
          公告发布
        </el-menu-item>

        <el-menu-item v-if="isAdmin" index="/reportManage">
          举报处理
        </el-menu-item>

        <!-- ================== 食堂管理员 ================== -->
        <el-menu-item v-if="isCanteenAdmin" index="/myCanteen">
          我的食堂
        </el-menu-item>

        <el-menu-item v-if="isCanteenAdmin" index="/menuManage">
          每日餐单
        </el-menu-item>

        <el-menu-item v-if="isCanteenAdmin" index="/dishManage">
          菜品管理
        </el-menu-item>

        <el-menu-item v-if="isCanteenAdmin" index="/purchase">
          日常采购
        </el-menu-item>

        <el-menu-item v-if="isCanteenAdmin" index="/financialStatistics">
          财务统计
        </el-menu-item>

        <el-menu-item v-if="isCanteenAdmin" index="/canteenOrders">
          食堂订单
        </el-menu-item>

        <el-menu-item v-if="isCanteenAdmin" index="/messageBoard">
          留言板
        </el-menu-item>

        <!-- ================== 所有用户 ================== -->
        <el-menu-item index="/userCenter">用户中心</el-menu-item>
      </el-menu>
    </el-aside>

    <!-- 右侧 -->
    <el-container>
      <el-header style="display:flex;align-items: center;gap: 15px;justify-content: flex-end;">
        <span>{{ user.username }}</span>
        <el-button type="danger" size="small" @click="logout">
          退出
        </el-button>
      </el-header>

      <el-main>
        <router-view />
      </el-main>
    </el-container>

  </el-container>
</template>

<script setup>
import { ref, onMounted, computed } from 'vue'
import { useRouter } from 'vue-router'

const router = useRouter()

const user = ref({
  username: '',
  role: '',
  adminLevel: 0,  // 管理员级别：1-系统管理员，2-市级管理员，3-区级管理员
  regionId: 0,    // 管理区域ID
  regionName: ''  // 管理区域名称
})

onMounted(() => {
  const u = localStorage.getItem('user')
  if (u) {
    user.value = { ...user.value, ...JSON.parse(u) }
  } else {
    router.push('/login')
  }
})

/* ================== 角色判断（推荐这样写） ================== */

// 用餐者
const isDiner = computed(() => user.value.role === 'diner')

// 系统管理员
const isSystemAdmin = computed(() => user.value.role === 'system_admin' || user.value.adminLevel === 1)

// 市级管理员
const isCityAdmin = computed(() => user.value.role === 'admin' && user.value.adminLevel === 2)

// 区级管理员
const isDistrictAdmin = computed(() => user.value.role === 'admin' && user.value.adminLevel === 3)

// 普通管理员（包含系统管理员、市级管理员、区级管理员）
const isAdmin = computed(() =>
  user.value.role === 'system_admin' ||
  (user.value.role === 'admin' && user.value.adminLevel > 0)
)

// 食堂管理员
const isCanteenAdmin = computed(() =>
  user.value.role === 'canteen_manager'
)

// 可以审核管理员申请（系统管理员+市级管理员）
const canReviewAdminApply = computed(() => isSystemAdmin.value || isCityAdmin.value)

// 可以审核食堂管理者申请（系统管理员+市级管理员+区级管理员）
const canReviewManagerApply = computed(() => isSystemAdmin.value || isCityAdmin.value || isDistrictAdmin.value)

// 可以管理管理员（系统管理员+市级管理员）
const canManageAdmin = computed(() => isSystemAdmin.value || isCityAdmin.value)

const logout = () => {
  localStorage.removeItem('user')
  user.value = {
    username: '',
    role: '',
    adminLevel: 0,
    regionId: 0,
    regionName: ''
  }
  router.push('/login')
}
</script>
