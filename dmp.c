#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/device-mapper.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <asm/string.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/string.h>
#include <linux/kobject.h>
struct dm_dev* dev;
size_t r,w,ar,aw;
char isReqUpd;
unsigned char len;
char statBuf[0xff]="Hello";
int ctr(struct dm_target *ti, unsigned int argc, char **argv)
{	
	if(argc!=1){
		printk("Invalid args!");
		return -1;
	}
	if(!dm_get_device(ti,*argv,dm_table_get_mode(ti->table),&dev)){
		printk("Dev init!");
		return 0;
	}else{
		printk("Invalid args!");
		return -1;
	}
}
int map(struct dm_target *ti, struct bio *bio)
{
	printk("Map %d",bio_op(bio));
	switch(bio_op(bio)){
		case REQ_OP_WRITE:{
			w++;
			aw+=bio->bi_io_vec->bv_len;
			break;
		}
		case REQ_OP_READ:{
			r++;
			ar+=bio->bi_io_vec->bv_len;
			break;
		}
	}
	bio_set_dev(bio,dev->bdev);
	submit_bio(bio);
	isReqUpd=1;
	return DM_MAPIO_SUBMITTED;
}
struct target_type tp={
	.name="dmp",
	.version={13,13,13},
	.module=THIS_MODULE,
	.ctr=ctr,
	.map=map
};
static struct kobject* stat;
static ssize_t getStat(struct kobject *kobj, struct kobj_attribute *attr,char *buf){
	if(isReqUpd){
		isReqUpd=0;
		size_t tot=r+w;
		if(!tot){
			tot=1;
		}
		sprintf(statBuf,"Read:\n\treqs: %lu\n\tavg size: %lu\nWrite:\n\treqs: %lu\n\tavg size: %lu\nTotal:\n\treqs: %lu\n\tavg size: %lu\n\t",r,r?ar/r:0,w,w?aw/w:0,w&&r?tot:0,(ar/tot+aw/tot));
		char*g=statBuf;
		len=0;
		while(*g){
			len++;
			g++;
		}
		len--;
	}
	strncpy(buf,statBuf,len);
	return len;
}
static struct kobj_attribute at=__ATTR(volumes,0444,getStat,0);
static struct attribute *reg[]={
	&at.attr,
	0
};
static struct attribute_group ag={
	.attrs=reg
};
int init_module()
{
	len=r=w=ar=aw=0;
	isReqUpd=1;
	int res=dm_register_target(&tp);
	if(res<0){
		goto er;
	}
	stat=kobject_create_and_add("stat",0);
	if(!stat){
		goto er;
	}
	if(sysfs_create_group(stat,&ag)){
		kobject_put(stat);
		goto er;
	}
	printk("Init DMP complete");
	return 0;
	er:
	printk("Error");
	return res;
}
void cleanup_module()
{
	kobject_put(stat);
	dm_unregister_target(&tp);
	printk("Goodbuy DMP!");
}
MODULE_LICENSE("Dual BSD/GPL");
