#include "list.h"
#include "interrupt.h"
void list_init (struct list* l){
	l->head.next=&l->tail;
	l->tail.prev=&l->head;
	l->head.prev=NULL;
	l->tail.next=NULL;
}
void list_insert_before(struct list_elem* before, struct list_elem* elem){

	enum intr_statu old_status=intr_disable();


	before->prev->next=elem;
	elem->prev=before->prev;
	elem->next=before;
	before->prev=elem;

	intr_set_status(old_status);

}
void list_push(struct list* plist, struct list_elem* elem){
	list_insert_before(plist->head.next,elem);
}
void list_iterate(struct list* plist){

}
void list_append(struct list* plist, struct list_elem* elem){


	list_insert_before(&plist->tail,elem);


}
void list_remove(struct list_elem* pelem){
	enum intr_statu old_status=intr_disable();

	pelem->prev->next=pelem->next;
	pelem->next->prev=pelem->prev;

	intr_set_status(old_status);
}
struct list_elem* list_pop(struct list* plist){
	struct list_elem* res=plist->head.next;
	list_remove(plist->head.next);

	return res;

};
bool list_empty(struct list* plist){
	return (plist->head.next==&plist->tail);
}
uint32_t list_len(struct list* plist){
	uint32_t len=0;
	struct list_elem * p=plist->head.next;
	while(p!=&plist->tail){
		len++;
		p=p->next;
	}
	return len;
}
struct list_elem* list_traversal(struct list* plist, function func, int arg){
	struct list_elem *p=plist->head.next;
	while(p!=&plist->tail){
		if(func(p,arg)){
			return p;
		}
		p=p->next;
	}
	return NULL;

};
bool elem_find(struct list* plist, struct list_elem* obj_elem){
	struct list_elem *p=&plist->head;
	while(p){
		if(p==obj_elem) return true;
		p=p->next;
	}
	return false;
}

