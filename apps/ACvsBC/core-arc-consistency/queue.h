// Part of https://github.com/pothitos/naxos

#ifndef Ns_QUEUE_H
#define Ns_QUEUE_H

template <class TemplType>
class Queue {
    private:
        class queuenode {
            public:
                TemplType thedata;
                queuenode* next;
                queuenode(const TemplType& T) : thedata(T), next(0)
                {
                }
        };

        queuenode *start, *end;

    public:
        Queue(void) : start(0), end(0)
        {
        }

        ~Queue(void)
        {
                clear();
        }

        void clear(void);

        bool empty(void)
        {
                return (start == 0);
        }

        TemplType& top(void)
        {
                assert_exc(!empty(), "Queue::top: Queue is empty");
                return start->thedata;
        }

        void pop(void);

        void push(const TemplType newdata);
};

template <class TemplType>
void Queue<TemplType>::clear(void)
{
        while (!empty())
                pop();
}

template <class TemplType>
void Queue<TemplType>::pop(void)
{
        assert_exc(!empty(), "Queue::pop: Queue is empty");
        queuenode* temp = start;
        start = start->next;
        if (start == 0)
                end = 0;
        delete temp;
}

template <class TemplType>
void Queue<TemplType>::push(const TemplType newdata)
{
        queuenode* newnode = new queuenode(newdata);
        // newnode->thedata = newdata;
        assert_exc(end == 0 || end->next == 0,
                   "Queue::push: Wrong end pointer/node");
        if (end != 0) {
                end->next = newnode;
        } else {
                assert_exc(start == 0, "Queue::push: Wrong start pointer");
                start = newnode;
        }
        end = newnode;
}
#endif // Ns_QUEUE_H
