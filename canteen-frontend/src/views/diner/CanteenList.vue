<template>
  <div>
    <h2>食堂列表</h2>

    <el-table :data="canteens" style="width: 100%">
      <el-table-column prop="id" label="ID" width="100" />
      <el-table-column prop="name" label="食堂名称" />

      <el-table-column label="操作">
        <template #default="scope">
          <el-button type="primary" @click="viewMenu(scope.row)">
            查看菜单
          </el-button>
        </template>
      </el-table-column>
    </el-table>

  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import axios from 'axios'
import { useRouter } from 'vue-router'

const router = useRouter()
const canteens = ref([])

// 获取食堂列表
onMounted(async () => {
  const res = await axios.get('http://192.168.56.100:8080/canteens')
  canteens.value = res.data.data
})

// 查看菜单
const viewMenu = (canteen) => {
  router.push({
    path: '/menu',
    query: {
      canteen_id: canteen.id
    }
  })
}
</script>