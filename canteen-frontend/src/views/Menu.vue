<template>
  <div>
    <h2>菜单列表</h2>

    <el-table :data="dishes" style="width: 100%">
      <el-table-column prop="id" label="ID" width="100" />
      <el-table-column prop="name" label="菜品名称" />
      <el-table-column prop="price" label="价格" />
    </el-table>

  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import axios from 'axios'
import { useRoute } from 'vue-router'

const route = useRoute()
const dishes = ref([])

onMounted(async () => {
  const canteen_id = route.query.canteen_id

  const res = await axios.get('http://192.168.56.100:8080/menu', {
    params: {
      canteen_id: canteen_id,
      date: '2026-03-30' // 可以后面动态改
    }
  })

  dishes.value = res.data.data
})
</script>