import { createRouter, createWebHistory } from 'vue-router'

import Login from '../views/Login.vue'
import Home from '../views/Home.vue'
import CanteenList from '../views/CanteenList.vue'
import Menu from '../views/Menu.vue'

const routes = [
  { path: '/', redirect: '/home' },
  { path: '/login', component: Login },
  { path: '/home', component: Home },
  { path: '/canteens', component: CanteenList },
  { path: '/menu', component: Menu },
]

const router = createRouter({
  history: createWebHistory(),
  routes
})

// ⭐ 登录拦截（核心🔥）
router.beforeEach((to, from, next) => {
  const user = localStorage.getItem('user')

  if (to.path === '/login') {
    next()
  } else {
    if (user) {
      next() // 已登录
    } else {
      next('/login') // 未登录
    }
  }
})

export default router