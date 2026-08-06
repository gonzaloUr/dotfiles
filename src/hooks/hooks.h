void* pa_init_component(int pipefd[2]);
void pa_start_component(void *userdata);
void pa_stop_component(void *userdata);
void pa_free_component(void *userdata);
