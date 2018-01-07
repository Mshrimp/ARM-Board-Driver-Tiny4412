#ifndef	__COMMON_H__
#define	__COMMON_H__



// 容错宏
#define	ERRP(con, module, func, ret)	do{													\
											if (con)										\
											{												\
												printf("%s: %s failed!\n", module, func);	\
												ret;										\
											}												\
										}while(0)

#define	ERRP_K(con, module, func, ret)	do{													\
											if (con)										\
											{												\
												printk("%s: %s failed!\n", module, func);	\
												ret;										\
											}												\
										}while(0)


#endif	/* __COMMON_H__ */
