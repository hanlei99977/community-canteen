import { createRouter, createWebHistory } from 'vue-router'

import Layout from '../layout/Layout.vue'
// common
import Login from '../views/common/Login.vue'
import Register from '../views/common/Register.vue'
import Home from '../views/common/Home.vue'
import UserCenter from '../views/common/UserCenter.vue'
// admin
import AdminManage from '../views/admin/AdminManage.vue'
import DinerManage from '../views/admin/DinerManage.vue'
// manager
import MenuManage from '../views/manager/MenuManage.vue'
import DishManage from '../views/manager/DishManage.vue'
// diner
import CanteenList from '../views/diner/CanteenList.vue'
import Menu from '../views/diner/Menu.vue'
import Orders from '../views/diner/Orders.vue'

const routes = [
  { path: '/login', component: Login },
  { path: '/register', component: Register },

  {path: '/',
    component: Layout,
    children: 
    [
      { path: '/home', component: Home },
      { path: '/canteens', component: CanteenList },
      { path: '/menu', component: Menu },
      { path: '/orders', component: Orders},
      { path: '/userCenter', component: UserCenter},
      { path: '/menuManage', component: MenuManage},
      { path: '/dishManage', component: DishManage},
      { path: '/adminManage', component: AdminManage},
      { path: '/dinerManage', component: DinerManage}
    ]
  }
]

const router = createRouter({
  history: createWebHistory(),
  routes
})

// ⭐ 登录拦截（核心🔥）
router.beforeEach((to, from) => {
  const user = localStorage.getItem('user')

  // 已登录不允许去登录页
  if ((to.path === '/login' || to.path === '/register') && user) {
    return '/home'
  }

  // 白名单
  if (to.path === '/login' || to.path === '/register') {
    return true
  }

  // 未登录
  if (!user) {
    return '/login'
  }

  return true
})
export default router