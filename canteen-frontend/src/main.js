import { createApp } from 'vue'
import App from './App.vue'

// ⭐ 路由
import router from './router'

// 引入 Element Plus
import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'

createApp(App)
  .use(router)
  .use(ElementPlus)
  .mount('#app')