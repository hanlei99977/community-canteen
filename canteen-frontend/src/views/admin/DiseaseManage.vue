<template>
  <div class="disease-manage">
    <h2>疾病管理</h2>
    
    <el-button type="primary" @click="openAddDialog">添加疾病</el-button>
    
    <el-table :data="diseaseList" border style="width: 100%;margin-top:20px;">
      <el-table-column prop="disease_id" label="疾病ID" />
      <el-table-column prop="disease_name" label="疾病名称" />
      <el-table-column label="关联标签" min-width="300">
        <template #default="scope">
          <div v-if="scope.row.tags && scope.row.tags.length > 0">
            <el-tag 
              v-for="tag in scope.row.tags" 
              :key="tag.tag_id"
              :type="tag.rule_type === 1 ? 'success' : 'warning'"
              style="margin-right: 5px;"
            >
              {{ tag.tag_name }} 
              <span style="font-size: 12px;">({{ tag.rule_type === 1 ? '推荐' : '不推荐' }})</span>
            </el-tag>
          </div>
          <span v-else style="color: #909399;">暂无标签</span>
        </template>
      </el-table-column>
      <el-table-column label="操作">
        <template #default="scope">
          <el-button size="small" @click="openEditDialog(scope.row)">修改</el-button>
        </template>
      </el-table-column>
    </el-table>

    <el-dialog title="添加疾病" v-model="addDialogVisible" width="600px">
      <el-form :model="addForm" label-width="120px">
        <el-form-item label="疾病名称">
          <el-input v-model="addForm.disease_name" />
        </el-form-item>
        <el-form-item label="关联标签">
          <div v-if="allTags.length > 0">
            <div v-for="tag in allTags" :key="tag.tag_id" style="margin-bottom: 10px;">
              <el-checkbox 
                v-model="addForm.selectedTags" 
                :label="tag.tag_id"
                @change="onAddTagChange(tag.tag_id)"
              >
                {{ tag.tag_name }}
              </el-checkbox>
              <el-radio-group 
                v-if="addForm.selectedTags.includes(tag.tag_id)"
                v-model="addForm.tagRuleTypes[tag.tag_id]"
              >
                <el-radio :label="1">推荐吃</el-radio>
                <el-radio :label="2">不推荐吃</el-radio>
              </el-radio-group>
            </div>
          </div>
          <span v-else>暂无标签数据</span>
          <p style="color: #909399; font-size: 12px; margin-top: 10px;">
            rule_type=1: 推荐吃此种口味的食物 | rule_type=2: 不推荐吃此种口味的食物
          </p>
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="addDialogVisible = false">取消</el-button>
        <el-button type="primary" @click="handleAdd">确认添加</el-button>
      </template>
    </el-dialog>

    <el-dialog title="修改疾病" v-model="editDialogVisible" width="600px">
      <el-form :model="editForm" label-width="120px">
        <el-form-item label="疾病名称">
          <el-input v-model="editForm.disease_name" />
        </el-form-item>
        <el-form-item label="关联标签">
          <div v-if="allTags.length > 0">
            <div v-for="tag in allTags" :key="tag.tag_id" style="margin-bottom: 10px;">
              <el-checkbox 
                v-model="editForm.selectedTags" 
                :label="tag.tag_id"
                @change="onEditTagChange(tag.tag_id)"
              >
                {{ tag.tag_name }}
              </el-checkbox>
              <el-radio-group 
                v-if="editForm.selectedTags.includes(tag.tag_id)"
                v-model="editForm.tagRuleTypes[tag.tag_id]"
              >
                <el-radio :label="1">推荐吃</el-radio>
                <el-radio :label="2">不推荐吃</el-radio>
              </el-radio-group>
            </div>
          </div>
          <span v-else>暂无标签数据</span>
          <p style="color: #909399; font-size: 12px; margin-top: 10px;">
            rule_type=1: 推荐吃此种口味的食物 | rule_type=2: 不推荐吃此种口味的食物
          </p>
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="editDialogVisible = false">取消</el-button>
        <el-button type="primary" @click="handleEdit">确认修改</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

const diseaseList = ref([])
const allTags = ref([])

const addDialogVisible = ref(false)
const editDialogVisible = ref(false)

const addForm = reactive({
  disease_name: '',
  selectedTags: [],
  tagRuleTypes: {}
})

const editForm = reactive({
  disease_id: 0,
  disease_name: '',
  selectedTags: [],
  tagRuleTypes: {}
})

const onAddTagChange = (tagId) => {
  if (!addForm.tagRuleTypes[tagId]) {
    addForm.tagRuleTypes[tagId] = 1
  }
}

const onEditTagChange = (tagId) => {
  if (!editForm.tagRuleTypes[tagId]) {
    editForm.tagRuleTypes[tagId] = 1
  }
}

const loadDiseaseList = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/diseaseList')
    if (res.data.code === 0) {
      diseaseList.value = res.data.data
    }
  } catch (error) {
    ElMessage.error('加载疾病列表失败')
  }
}

const loadAllTags = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/getAllTags')
    if (res.data.code === 0) {
      allTags.value = res.data.data
    }
  } catch (error) {
    ElMessage.error('加载标签列表失败')
  }
}

const openAddDialog = () => {
  addForm.disease_name = ''
  addForm.selectedTags = []
  addForm.tagRuleTypes = {}
  addDialogVisible.value = true
}

const openEditDialog = (row) => {
  editForm.disease_id = row.disease_id
  editForm.disease_name = row.disease_name
  editForm.selectedTags = row.tags ? row.tags.map(t => t.tag_id) : []
  editForm.tagRuleTypes = {}
  if (row.tags) {
    row.tags.forEach(tag => {
      editForm.tagRuleTypes[tag.tag_id] = tag.rule_type
    })
  }
  editDialogVisible.value = true
}

const handleAdd = async () => {
  if (!addForm.disease_name.trim()) {
    ElMessage.error('请输入疾病名称')
    return
  }
  
  const tags = addForm.selectedTags.map(tagId => ({
    tag_id: tagId,
    rule_type: addForm.tagRuleTypes[tagId] || 1
  }))
  
  try {
    const res = await axios.post('http://192.168.56.100:8080/diseaseCreate', {
      disease_name: addForm.disease_name,
      tags
    })
    if (res.data.code === 0) {
      ElMessage.success('添加成功')
      addDialogVisible.value = false
      loadDiseaseList()
    } else {
      ElMessage.error('添加失败')
    }
  } catch (error) {
    ElMessage.error('添加失败')
  }
}

const handleEdit = async () => {
  if (!editForm.disease_name.trim()) {
    ElMessage.error('请输入疾病名称')
    return
  }
  
  const tags = editForm.selectedTags.map(tagId => ({
    tag_id: tagId,
    rule_type: editForm.tagRuleTypes[tagId] || 1
  }))
  
  try {
    const res = await axios.post('http://192.168.56.100:8080/diseaseUpdate', {
      disease_id: editForm.disease_id,
      disease_name: editForm.disease_name,
      tags
    })
    if (res.data.code === 0) {
      ElMessage.success('修改成功')
      editDialogVisible.value = false
      loadDiseaseList()
    } else {
      ElMessage.error('修改失败')
    }
  } catch (error) {
    ElMessage.error('修改失败')
  }
}

onMounted(() => {
  loadDiseaseList()
  loadAllTags()
})
</script>