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
import AdminApply from '../views/admin/AdminApply.vue'
import ManagerApply from '../views/admin/ManagerApply.vue'
import Announcement from '../views/admin/Announcement.vue'
import ReportManage from '../views/admin/ReportManage.vue'
import CanteenManage from '../views/admin/CanteenManage.vue'
// manager
import MenuManage from '../views/manager/MenuManage.vue'
import DishManage from '../views/manager/DishManage.vue'
import MyCanteen from '../views/manager/MyCanteen.vue'
import Purchase from '../views/manager/Purchase.vue'
import FinancialStatistics from '../views/manager/FinancialStatistics.vue'
import MessageBoard from '../views/manager/MessageBoard.vue'
import CanteenOrders from '../views/manager/CanteenOrders.vue'
// diner
import CanteenList from '../views/diner/CanteenList.vue'
import Menu from '../views/diner/Menu.vue'
import Orders from '../views/diner/Orders.vue'
import DiningPreference from '../views/diner/DiningPreference.vue'
import MessageCenter from '../views/diner/MessageCenter.vue'

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
      { path: '/diningPreference', component: DiningPreference},
      { path: '/messageCenter', component: MessageCenter},
      { path: '/myCanteen', component: MyCanteen},
      { path: '/menuManage', component: MenuManage},
      { path: '/dishManage', component: DishManage},
      { path: '/purchase', component: Purchase},
      { path: '/financialStatistics', component: FinancialStatistics},
      { path: '/canteenOrders', component: CanteenOrders},
      { path: '/messageBoard', component: MessageBoard},
      { path: '/adminManage', component: AdminManage},
      { path: '/adminApply', component: AdminApply},
      { path: '/managerApply', component: ManagerApply},
      { path: '/dinerManage', component: DinerManage},
      { path: '/announcement', component: Announcement},
      { path: '/reportManage', component: ReportManage},
      { path: '/canteenManage', component: CanteenManage}
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
