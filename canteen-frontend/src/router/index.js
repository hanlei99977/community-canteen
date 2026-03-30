import { createRouter, createWebHistory } from 'vue-router'

import Login from '../views/Login.vue'
import Register from '../views/Register.vue'
import Home from '../views/Home.vue'
import CanteenList from '../views/CanteenList.vue'
import Menu from '../views/Menu.vue'
import Layout from '../layout/Layout.vue'

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