1.
/*
 * Get number of arguments passed to function.
 */
#define PG_NARGS() (fcinfo->nargs)

在系统函数如下获取入参个数
Datum
dblink_connect(PG_FUNCTION_ARGS)

2.
#define makeNode(_type_)		((_type_ *) newNode(sizeof(_type_),T_##_type_))
#define NodeSetTag(nodeptr,t)	(((Node*)(nodeptr))->type = (t))

#define IsA(nodeptr,_type_)		(nodeTag(nodeptr) == T_##_type_)

：： 看到这里应该记得postgresql源代码中有很多这样的变量T_xxxx，可以看下nodes/nodes.h文件，而这些变量都是与哪些对应的呢？
其实这和连接字符串“##”密不可分。

在C/C++的宏中，”#”的功能是将其后面的宏参数进行字符串化操作(Stringfication)，简单说就是在对它所引用的宏变量通过替换后在其左右
各加上一个双引号。而”##”被称为连接符(concatenator)，用来将两个子串Token连接为一个Token。注意这里连接的对象是Token就行，而不一
定是宏的变量。还可以n个##符号连接n+1个Token，这个特性是#符号所不具备的。

3.断言语句
Assert(rsinfo->returnMode == SFRM_Materialize);

4.
PG_TRY();
{}
PG_CATCH();
{}
PG_END_TRY();

5.
postgresql 内存上下文切换
	MemoryContext ind_context,
				old_context;
ind_context = AllocSetContextCreate(anl_context,
										"Analyze Index",
										ALLOCSET_DEFAULT_MINSIZE,
										ALLOCSET_DEFAULT_INITSIZE,
										ALLOCSET_DEFAULT_MAXSIZE);
	old_context = MemoryContextSwitchTo(ind_context);
	.....
	MemoryContextSwitchTo(old_context);
	MemoryContextDelete(ind_context);
	在中间地带申请的内存，即使不是放，也会随着MemoryContextSwitchTo 切换会old_context而释放，注意create的context需要释放掉。
	
6. 表扫描
/* Prepare to scan pg_index for entries having indrelid = this rel. */
	indexRelation = heap_open(IndexRelationId, AccessShareLock);
	ScanKeyInit(&skey,
				Anum_pg_index_indrelid,
				BTEqualStrategyNumber, F_OIDEQ,
				ObjectIdGetDatum(RelationGetRelid(rel)));

	scan = systable_beginscan(indexRelation, IndexIndrelidIndexId, true,
							  SnapshotNow, 1, &skey);

	while (HeapTupleIsValid(indexTuple = systable_getnext(scan)))
	{
		Form_pg_index index = (Form_pg_index) GETSTRUCT(indexTuple);

		/* we're only interested if it is the primary key */
		if (index->indisprimary)
		{
		}
	}

	systable_endscan(scan);
	heap_close(indexRelation, AccessShareLock);
	
7. 给字符串加引号
quote_ident_cstr(NameStr(tupdesc->attrs[i]->attname))

	
